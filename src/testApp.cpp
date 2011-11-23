#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup(){

	ofSetLogLevel(OF_LOG_VERBOSE);
	ofBackground(0,0,0);
	ofSetFrameRate(30);
	ofEnableAlphaBlending();

	shader.load("shaders/forceExpression.vert","shaders/forceExpression.frag");

    fbo.allocate(XRES, YRES, GL_RGBA, FSAA);

    receiver.setup(PORT);
    sender.setup( HOST, PORTOUT );

    generateCode();
    resetState();

    bSaverMode = false;

}

//--------------------------------------------------------------
void testApp::update(){

    doOSC();

    TickIncreasingIndex();

    if(bNewCode) { generateCode(); CreateAndSendMathString(); SendBigCode(); }
    if(bMutateCode) { mutateCode(); CreateAndSendMathString(); SendBigCode();  }

    ofFill();
    fbo.begin();
    ofClear(0,0,0,1);
    shader.begin();
        shader.setUniform1f("time", increasingIndex);
        shader.setUniform1f("redLevel", redLevel);      // cut certain channels
        shader.setUniform1f("greenLevel", greenLevel);
        shader.setUniform1f("blueLevel", blueLevel);
        shader.setUniform1i("bIsBlackAndWhite", bIsBlackAndWhite);  // "focus mode"
        shader.setUniform2f("resolution", ofGetWidth(), ofGetHeight());
        shader.setUniform2f("mouse", terminalX, terminalY);
        shader.setUniform1iv("operators", operatorSwitches, 8);
        shader.setUniform1iv("code",code,CODE_LENGTH);

        ofSetColor(0,0,0);
        ofRect(0,0,ofGetWidth(), ofGetHeight());

    shader.end();
    fbo.end();

    ofSetWindowTitle(ofToString(ofGetFrameRate(),2));

}

//--------------------------------------------------------------
void testApp::draw(){
    ofSetColor(255,255,255,255 * masterAlpha);
    if(!bALLOFF) { fbo.draw(0,0,ofGetWidth(),ofGetHeight()); }
    if(bSaverMode) { ofSaveScreen(ofGetTimestampString()+".png"); }
}

//--------------------------------------------------------------
void testApp::keyPressed  (int key){

        if(key == 'f') { ofToggleFullscreen(); }

        if ( key =='a' || key == 'A' ){
            // save every frame of a program through each step of the increasing index
            increasingIndex = 0.0;
            bSaverMode = true;

        }

        if ( key == ' '){ ofSaveScreen(ofGetTimestampString()+".png"); }

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){
}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){
}

