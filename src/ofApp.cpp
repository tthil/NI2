#include "ofApp.h"

using namespace openni;
using namespace std;
//using namespace cv;

char ReadLastCharOfLine()
{
    int newChar = 0;
    int lastChar;
    fflush(stdout);

    do
    {
        lastChar = newChar;
        newChar = getchar();
    } while ((newChar != '\n') && (newChar != EOF));
    return (char)lastChar;
}

bool HandleStatus(Status status)
{
    if (status == STATUS_OK)
        return true;
    printf("ERROR: #%d, %s", status,
           OpenNI::getExtendedError());
    ReadLastCharOfLine();
    return false;
}

void ofApp::setup()
{
    Status status = STATUS_OK;
    printf("\r\n---------------------- Init OpenNI --------------------------\r\n");
    printf("Scanning machine for devices and loading "
           "modules/drivers ...\r\n");

    status = OpenNI::initialize();
    if (!HandleStatus(status))
        ofExit(1);
    printf("Completed.\r\n");

    printf("\r\n---------------------- Open Device --------------------------\r\n");
    printf("Opening first device ...\r\n");
    status = device.open(ANY_DEVICE);
    if (!HandleStatus(status))
    ofExit(1);
    printf("%s Opened, Completed.\r\n",
           device.getDeviceInfo().getName());

    printf("\r\n---------------------- Depth Stream --------------------------\r\n");
    printf("Checking if stream is supported ...\r\n");
    if (!device.hasSensor(SENSOR_DEPTH))
    {
        printf("Stream not supported by this device.\r\n");
        ofExit(1);
    }

    printf("Asking device to create a depth stream ...\r\n");
    status = depthSensor.create(device, SENSOR_DEPTH);
    if (!HandleStatus(status))
    ofExit(1);

    printf("Setting video mode to 640x480x30 Depth 1MM ...\r\n");
    VideoMode vmod;
    vmod.setFps(30);
    vmod.setPixelFormat(PIXEL_FORMAT_DEPTH_1_MM);
    vmod.setResolution(640, 480);
    status = depthSensor.setVideoMode(vmod);
    if (!HandleStatus(status))
    ofExit(1);
    printf("Done.\r\n");

    printf("Starting stream ...\r\n");
    status = depthSensor.start();
    if (!HandleStatus(status))
    ofExit(1);
    printf("Done.\r\n");

    printf("\r\n---------------------- OpenGL -------------------------\r\n");
    printf("Initializing OpenGL ...\r\n");
    gl_texture = (OniRGB888Pixel *)malloc(
        window_w * window_h * sizeof(OniRGB888Pixel));
//    glutInit(&argc, (char **)argv);
/*    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(window_w, window_h);
    glutCreateWindow("OpenGL | OpenNI 2.x CookBook Sample");
    glutKeyboardFunc(gl_KeyboardCallback);
    glutDisplayFunc(gl_DisplayCallback);
    glutIdleFunc(gl_IdleCallback);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    printf("Starting OpenGL rendering process ...\r\n");
    glutMainLoop();
*/
}

void ofApp::update()
{
}

void ofApp::draw()
{
    if (depthSensor.isValid())
    {
        Status status = STATUS_OK;
        VideoStream *streamPointer = &depthSensor;
        int streamReadyIndex;
        status = OpenNI::waitForAnyStream(&streamPointer, 1,
                                          &streamReadyIndex, 500);
        if (status == STATUS_OK && streamReadyIndex == 0)
        {
            VideoFrameRef newFrame;
            status = depthSensor.readFrame(&newFrame);
            if (status == STATUS_OK && newFrame.isValid())
            {
                // Clear the OpenGL buffers
                glClear(
                    GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                // Setup the OpenGL viewpoint
                glMatrixMode(GL_PROJECTION);
                glPushMatrix();
                glLoadIdentity();
                glOrtho(0, window_w, window_h, 0, -1.0, 1.0);

                // UPDATING TEXTURE (DEPTH 1MM TO RGB888)
                unsigned short maxDepth =
                    depthSensor.getMinPixelValue();
                unsigned short minDepth =
                    depthSensor.getMaxPixelValue();
                for (int y = 0; y < newFrame.getHeight(); ++y)
                {
                    DepthPixel *depthCell = (DepthPixel *)((char *)newFrame.getData() +
                                                           (y * newFrame.getStrideInBytes()));
                    for (int x = 0; x < newFrame.getWidth();
                         ++x, ++depthCell)
                    {
                        if (maxDepth < *depthCell)
                        {
                            maxDepth = *depthCell;
                        }
                        if (*depthCell != 0 &&
                            minDepth > *depthCell)
                        {
                            minDepth = *depthCell;
                        }
                    }
                }

                int depthHistogram[65536];
                int numberOfPoints = 0;
                if (histogram_enable)
                {
                    memset(depthHistogram, 0,
                           sizeof(depthHistogram));
                    for (int y = 0;
                         y < newFrame.getHeight(); ++y)
                    {
                        DepthPixel *depthCell = (DepthPixel *)((char *)newFrame.getData() +
                                                               (y * newFrame.getStrideInBytes()));
                        for (int x = 0; x < newFrame.getWidth();
                             ++x, ++depthCell)
                        {
                            if (*depthCell != 0)
                            {
                                depthHistogram[*depthCell]++;
                                numberOfPoints++;
                            }
                        }
                    }

                    for (int nIndex = 1;
                         nIndex < sizeof(depthHistogram) / sizeof(int);
                         nIndex++)
                    {
                        depthHistogram[nIndex] +=
                            depthHistogram[nIndex - 1];
                    }
                }

                double resizeFactor = min(
                    (window_w / (double)newFrame.getWidth()),
                    (window_h / (double)newFrame.getHeight()));
                unsigned int texture_x = (unsigned int)(window_w -
                                                        (resizeFactor * newFrame.getWidth())) /
                                         2;
                unsigned int texture_y = (unsigned int)(window_h -
                                                        (resizeFactor * newFrame.getHeight())) /
                                         2;

                for (unsigned int y = 0;
                     y < (window_h - 2 * texture_y); ++y)
                {
                    OniRGB888Pixel *texturePixel = gl_texture +
                                                   ((y + texture_y) * window_w) + texture_x;
                    DepthPixel lastPixel = 0;
                    for (unsigned int x = 0;
                         x < (window_w - 2 * texture_x);
                         ++x, ++texturePixel)
                    {
                        DepthPixel *streamPixel =
                            (DepthPixel *)((char *)newFrame.getData() +
                                           ((int)(y / resizeFactor) *
                                            newFrame.getStrideInBytes())) +
                            (int)(x / resizeFactor);
                        if (*streamPixel != 0)
                        {
                            lastPixel = *streamPixel;
                        }
                        else if (!blackfill_enable)
                        {
                            lastPixel = 0;
                        }
                        if (lastPixel != 0)
                        {
                            char depthValue = ((float)lastPixel /
                                               maxDepth) *
                                              255;
                            if (color_enable)
                            {
                                float colorPaletteFactor =
                                    (float)1024 / maxDepth;
                                int colorCode =
                                    (lastPixel - minDepth) *
                                    colorPaletteFactor;
                                texturePixel->b = ((colorCode > 0 && colorCode < 512) ? abs(colorCode - 256) : 255);
                                texturePixel->g = ((colorCode > 128 && colorCode < 640) ? abs(colorCode - 384) : 255);
                                texturePixel->r = ((colorCode > 512 && colorCode < 1024) ? abs(colorCode - 768) : 255);
                            }
                            else
                            {
                                if (histogram_enable)
                                {
                                    depthValue = ((float)depthHistogram[lastPixel] / numberOfPoints) * 255;
                                }
                                texturePixel->b = 255 - depthValue;
                                texturePixel->g = 255 - depthValue;
                                texturePixel->r = 255 - depthValue;
                            }
                        }
                        else
                        {
                            texturePixel->b = lastPixel;
                            texturePixel->g = lastPixel;
                            texturePixel->r = lastPixel;
                        }
                    }
                }

                // Create the OpenGL texture map
                glTexParameteri(GL_TEXTURE_2D,
                                0x8191, GL_TRUE); // 0x8191 = GL_GENERATE_MIPMAP
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                             window_w, window_h, 0, GL_RGB,
                             GL_UNSIGNED_BYTE, gl_texture);

                glBegin(GL_QUADS);
                glTexCoord2f(0.0f, 0.0f);
                glVertex3f(0.0f, 0.0f, 0.0f);
                glTexCoord2f(0.0f, 1.0f);
                glVertex3f(0.0f, (float)window_h, 0.0f);
                glTexCoord2f(1.0f, 1.0f);
                glVertex3f((float)window_w,
                           (float)window_h, 0.0f);
                glTexCoord2f(1.0f, 0.0f);
                glVertex3f((float)window_w, 0.0f, 0.0f);
                glEnd();

//                glutSwapBuffers();
            }
        }
    }
}

void ofApp::keyPressed(int key)
{
    if (key == 27) // ESC Key
    {
        depthSensor.destroy();
        OpenNI::shutdown();
        ofExit(0);
    }
    else if (key == 72 || key == 104) // H or h key
    {
        histogram_enable = !histogram_enable;
        color_enable = false;
    }
    else if (key == 70 || key == 102) // F or f key
    {
        blackfill_enable = !blackfill_enable;
    }
    else if (key == 67 || key == 99) // C or c key
    {
        color_enable = !color_enable;
        histogram_enable = false;
    }
}

void ofApp::keyReleased(int key)
{
}

/*
//--------------------------------------------------------------
void ofApp::setup()
{
    vidGrabber.setVerbose(true);
    vidGrabber.setup(320, 240);

    colorImg.allocate(320, 240);
    grayImage.allocate(320, 240);
    grayBg.allocate(320, 240);
    grayDiff.allocate(320, 240);

    bLearnBakground = true;
    threshold = 80;
}

//--------------------------------------------------------------
void ofApp::update()
{
    ofBackground(100, 100, 100);

    bool bNewFrame = false;

    vidGrabber.update();
    bNewFrame = vidGrabber.isFrameNew();

    if (bNewFrame)
    {

        colorImg.setFromPixels(vidGrabber.getPixels());

        grayImage = colorImg;
        if (bLearnBakground == true)
        {
            grayBg = grayImage; // the = sign copys the pixels from grayImage into grayBg (operator overloading)
            bLearnBakground = false;
        }

        // take the abs value of the difference between background and incoming and then threshold:
        grayDiff.absDiff(grayBg, grayImage);
        grayDiff.threshold(threshold);

        // find contours which are between the size of 20 pixels and 1/3 the w*h pixels.
        // also, find holes is set to true so we will get interior contours as well....
        contourFinder.findContours(grayDiff, 20, (340 * 240) / 3, 10, true); // find holes
    }
}

//--------------------------------------------------------------
void ofApp::draw()
{
    // draw the incoming, the grayscale, the bg and the thresholded difference
    ofSetHexColor(0xffffff);
    colorImg.draw(20, 20);
    grayImage.draw(360, 20);
    grayBg.draw(20, 280);
    grayDiff.draw(360, 280);

    // then draw the contours:

    ofFill();
    ofSetHexColor(0x333333);
    ofDrawRectangle(360, 540, 320, 240);
    ofSetHexColor(0xffffff);

    // we could draw the whole contour finder
    //contourFinder.draw(360,540);

    // or, instead we can draw each blob individually from the blobs vector,
    // this is how to get access to them:
    for (int i = 0; i < contourFinder.nBlobs; i++)
    {
        contourFinder.blobs[i].draw(360, 540);

        // draw over the centroid if the blob is a hole
        ofSetColor(255);
        if (contourFinder.blobs[i].hole)
        {
            ofDrawBitmapString("hole",
                               contourFinder.blobs[i].boundingRect.getCenter().x + 360,
                               contourFinder.blobs[i].boundingRect.getCenter().y + 540);
        }
    }

    // finally, a report:
    ofSetHexColor(0xffffff);
    stringstream reportStr;
    reportStr << "bg subtraction and blob detection" << endl
              << "press ' ' to capture bg" << endl
              << "threshold " << threshold << " (press: +/-)" << endl
              << "num blobs found " << contourFinder.nBlobs << ", fps: " << ofGetFrameRate();
    ofDrawBitmapString(reportStr.str(), 20, 600);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key)
{
    switch (key)
    {
    case 27:
        OpenNI::shutdown();
        ofExit(0);
        break;
    case ' ':
        bLearnBakground = true;
        break;
    case '+':
        threshold++;
        if (threshold > 255)
            threshold = 255;
        break;
    case '-':
        threshold--;
        if (threshold < 0)
            threshold = 0;
        break;
    }
}
*/
//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y)
{
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button)
{
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button)
{
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button)
{
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y)
{
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y)
{
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h)
{
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg)
{
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo)
{
}
