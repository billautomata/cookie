#pragma once

/*
	Cookie - interactive generative visualization
		by Bill McKessy (Automata)
		http://university-records.com
		http://creative-co.de
*/

#include "ofMain.h"
#include "ofxOsc.h"

//  OSC input
#define PORT 9090
#define NUM_MSG_STRINGS 20

// OSC output
#define HOST "192.168.1.143"    // set the IP and port of your touchOSC host
#define PORTOUT 9091


#define CODE_LENGTH 196         /// Length of the code sent to the shader
#define XRES 1024   // resolution
#define YRES 768
#define FSAA 4     // FSAA, if your card can handle it, crank it up but 4 is sufficient

#define MUTATION_RATE 0.001

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed  (int key);
		void keyReleased (int key);

		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		ofShader shader;
		ofFbo fbo;

		ofxOscReceiver	receiver;
		ofxOscSender sender;

		int				current_msg_string;
		string		msg_strings[NUM_MSG_STRINGS];
		float			timers[NUM_MSG_STRINGS];

		int				mouseX, mouseY;
		string			mouseButtonState;

        float terminalX, terminalY;
        bool bALLOFF, bNewCode, bMutateCode;
        float masterAlpha;
        int operatorSwitches[8];    // turns certain operators on and off inside the shader
        bool bIncreasingIndexPause;
        bool bIncreasingIndexSlow;
        bool bIncreasingIndexFast;
        bool bResetAll;

        float increasingIndex; float increasingIndexMax;
        float redLevel, greenLevel, blueLevel;
        bool bIsBlackAndWhite;
        bool bIsAutoMode;           // impelement
        bool bSaverMode;

		int code[CODE_LENGTH]; string mathematicalOperations;

		void generateCode(){

            for(int i = 0; i < CODE_LENGTH; i++)
            {
                if(ofRandom(0,1) > 0.5) { code[i] = 0; } else { code[i] = 1; }
            }

            SendCodeOSC();

		}

		void CreateAndSendMathString(){

            mathematicalOperations="";
            int maxCrawlSize = CODE_LENGTH - 8;
            int colorIndex = 0;     // this is changed in the while loop and used to reference which color we are operating on

            bool colorBit0 = false; bool colorBit1 = false;
            bool operatorBit0 = false; bool operatorBit1 = false; bool operatorBit2 = false;
            bool terminalBit0 = false; bool terminalBit1 = false; bool terminalBit2 = false;

            int oscLineIndex = 1;
            int codeIndex = 0; // step through the code with this index, controls the while loop
            while(codeIndex < maxCrawlSize)
            {
                // initialize the variables
                colorBit0 = colorBit1 = operatorBit0 = operatorBit1 = operatorBit2 = terminalBit0 = terminalBit1 = terminalBit2 = false;
                colorIndex = 0;

                if(bIsBlackAndWhite == 0)
                {        // choose the color
                    if(code[codeIndex+0] != 0) { colorBit0 = true; }
                    if(code[codeIndex+1] != 0) { colorBit1 = true; }
                    codeIndex+=2;   // done choosing the color, shift the register head forward two places
                }
                if(colorBit0) { colorIndex+=2; }
                if(colorBit1) { colorIndex+=1; }



                string colorString = ""; string operationString = "";
                if(colorIndex== 0) { colorString = "red"; }
                else if(colorIndex == 1) { colorString = "green"; }
                else if(colorIndex == 2) { colorString = "blue"; }
                else if(colorIndex == 3) { colorString = "constant"; }

                if(bIsBlackAndWhite == 1) { colorString = "focusValue"; }

                // choose the operator
                if(code[codeIndex+0] != 0) { operatorBit0 = true; }
                if(code[codeIndex+1] != 0) { operatorBit1 = true; }
                if(code[codeIndex+2] != 0) { operatorBit2 = true; }
                codeIndex+=3;   // done picking the operator

                /// operator __value__

                if(!operatorBit0 && !operatorBit1 && !operatorBit2 && operatorSwitches[0])                   // 000
                {   /// SIN _runningTally[colorIndex]_
                    /// PERFORM THE OPERATION
                    operationString = colorString + "=sin(" + colorString + ")\n";
                }
                else if(!operatorBit0 && !operatorBit1 && operatorBit2 && operatorSwitches[1] )               // 001
                {   /// COS _runningTally[colorIndex]_
                    /// PERFORM THE OPERATION
                    operationString = colorString + "=cos(" + colorString + ")\n";
                }
                else if(!operatorBit0 && operatorBit1 && !operatorBit2 && operatorSwitches[2])               // 010
                {   /// fract _runningTally[colorIndex]_
                    /// PERFORM THE OPERATION
                    operationString = colorString + "=fract(" + colorString + ")\n";
                }
                else if(!operatorBit0 && operatorBit1 && operatorBit2 && operatorSwitches[3] )                // 011
                {   /// ADD _chosenValue_
                    /// currenValue + (a value from the list from the terminal set)
                    float valueToOperateOn = 0.0;
                    bool v0 = false; bool v1 = false;
                    if(code[codeIndex+0] != 0) { v0 = true; }
                    if(code[codeIndex+1] != 0) { v1 = true; }
                    codeIndex+=2;
                    if(!v0 && !v1) { operationString = colorString + "+=time\n";} /// operate on time
                    if(!v0 &&  v1) { operationString = colorString + "+=time\n";} /// operate on positionX
                    if( v0 && !v1) { operationString = colorString + "+=positionY\n";} /// operate on positionY
                    if( v0 &&  v1) { operationString = colorString + "+=constant\n"; } /// operate on runningTally[colorIndex]
                    /// PERFORM THE OPERATION
                }
                else if(operatorBit0 && !operatorBit1 && !operatorBit2 && operatorSwitches[4])               // 100
                {   /// SUB _chosenValue_
                    /// currenValue + (a value from the list from the terminal set)
                    bool v0 = false; bool v1 = false;
                    if(code[codeIndex+0] != 0) { v0 = true; }
                    if(code[codeIndex+1] != 0) { v1 = true; }
                    codeIndex+=2;
                    if(!v0 && !v1) { operationString = colorString + "-=time\n";} /// operate on time
                    if(!v0 &&  v1) { operationString = colorString + "-=positionX\n";} /// operate on positionX
                    if( v0 && !v1) { operationString = colorString + "+=positionY\n";} /// operate on positionY
                    if( v0 &&  v1) {  operationString = colorString + "-=time\n";} /// operate on runningTally[colorIndex]
                    /// PERFORM THE OPERATION
                }
                else if(operatorBit0 && !operatorBit1 && operatorBit2 && operatorSwitches[5])                // 101
                {   /// MULT _chosenValue_
                    /// currenValue + (a value from the list from the terminal set)
                    float valueToOperateOn = 0.0;
                    bool v0 = false; bool v1 = false;
                    if(code[codeIndex+0] != 0) { v0 = true; }
                    if(code[codeIndex+1] != 0) { v1 = true; }
                    codeIndex+=2;
                    if(!v0 && !v1) { operationString = colorString + "*=time\n";} /// operate on time
                    if(!v0 &&  v1) { operationString = colorString + "*=positionX\n";} /// operate on positionX
                    if( v0 && !v1) { operationString = colorString + "*=time\n"; } /// operate on positionY
                    if( v0 &&  v1) {  operationString = colorString + "*=constant\n";} /// operate on runningTally[colorIndex]
                    /// PERFORM THE OPERATION

                }
                else if(operatorBit0 && operatorBit1 && !operatorBit2 && operatorSwitches[6] )               // 110
                {   /// sqrt _runningTally[colorIndex]_
                    /// PERFORM THE OPERATION
                    operationString = colorString + "+=sin(distance(x,y))\n";

                }
                else if(operatorBit0 && operatorBit1 && operatorBit2 && operatorSwitches[7])               // 111
                {   /// fract _runningTally[colorIndex]_
                    /// PERFORM THE OPERATION
                    //runningTally[colorIndex] = fract(runningTally[colorIndex]);
                    operationString = colorString + "+=cos(distance(x,y))\n";
                }
                if(operationString != "")
                {
                    ofxOscMessage m;
                    m.setAddress( "/simulation/mathcode"+ofToString(oscLineIndex,0) ); oscLineIndex++;
                    m.addStringArg( operationString );
                    sender.sendMessage( m ); m.clear();
                }
            }   // end of while()
            while(oscLineIndex < 46)
            {
                ofxOscMessage m;
                m.setAddress( "/simulation/mathcode"+ofToString(oscLineIndex,0) ); oscLineIndex++;
                m.addStringArg( "" );
                sender.sendMessage( m ); m.clear();
                oscLineIndex++;
            }

		}

		void SendCodeOSC(){

            ofxOscMessage m;
            string codeString0 = "";
            string codeString1 = "";
            string codeString2 = "";
            string codeString3 = "";

            int cl4 = CODE_LENGTH / 4;

            for(int i = 0; i < cl4; i++)
            {
                codeString0 += ofToString(code[i],0);
            }
            for(int i = cl4; i < 2*cl4; i++)
            {
                codeString1 += ofToString(code[i],0);
            }
            for(int i = 2*cl4; i < 3*cl4; i++)
            {
                codeString2 += ofToString(code[i],0);
            }
            for(int i = 3*cl4; i < CODE_LENGTH ; i++)
            {
                codeString3 += ofToString(code[i],0);
            }

            m.setAddress( "/simulation/code0" );
            m.addStringArg( codeString0 );
            sender.sendMessage( m ); m.clear();
            m.setAddress( "/simulation/code1" );
            m.addStringArg( codeString1 );
            sender.sendMessage( m ); m.clear();
            m.setAddress( "/simulation/code2" );
            m.addStringArg( codeString2 );
            sender.sendMessage( m ); m.clear();
            m.setAddress( "/simulation/code3" );
            m.addStringArg( codeString3 );
            sender.sendMessage( m ); m.clear();

		}

        void resetState(){
            bResetAll = false;

            // initialize the variables and send the data out over osc
            terminalX = ofGetWidth()/2.0; terminalY = ofGetHeight()/2.0;
            bALLOFF = false; bNewCode = false; bMutateCode = false;
            masterAlpha = 1.0; operatorSwitches = {1,1,1,1,1,1,1,1};
            bIncreasingIndexPause = false; bIncreasingIndexFast = false; bIncreasingIndexSlow = false;
            increasingIndex = 0.0; increasingIndexMax = 10.0;
            redLevel = 1.0; greenLevel = 1.0; blueLevel = 1.0;
            bIsBlackAndWhite = false; bIsAutoMode = false;

            // do OSC stuff
            if(true)
            {
                if(true){   // XY pad
                ofxOscMessage m;
                m.setAddress( "/simulation/xy" );
                m.addFloatArg( 0.5 );
                m.addFloatArg( 0.5 );
                sender.sendMessage( m );
                }
                if(true){   // operators
                    for(int i = 0; i < 8; i++)
                    {
                        ofxOscMessage m;
                        m.setAddress( "/simulation/op"+ofToString(i,0) );
                        m.addFloatArg( operatorSwitches[i]);
                        sender.sendMessage( m );
                    }
                }
                if(true)
                {
                    ofxOscMessage m;
                    m.setAddress( "/simulation/alpha" );
                    m.addFloatArg( 1.0 );
                    sender.sendMessage( m ); m.clear();
                }
                if(true){   // increasing index buttons
                    ofxOscMessage m;
                    m.setAddress( "/simulation/increasingIndexPause" );
                    m.addFloatArg( 0 );
                    sender.sendMessage( m ); m.clear();
                    m.setAddress( "/simulation/indexFast" );
                    m.addFloatArg( 0 );
                    sender.sendMessage( m ); m.clear();
                    m.setAddress( "/simulation/indexSlow" );
                    m.addFloatArg( 0 );
                    sender.sendMessage( m ); m.clear();
                }
                if(true){   // channel levels
                    ofxOscMessage m;
                    m.setAddress( "/simulation/redLevel" );
                    m.addFloatArg( 1.0 );
                    sender.sendMessage( m ); m.clear();
                    m.setAddress( "/simulation/greenLevel" );
                    m.addFloatArg( 1.0 );
                    sender.sendMessage( m ); m.clear();
                    m.setAddress( "/simulation/blueLevel" );
                    m.addFloatArg( 1.0 );
                    sender.sendMessage( m ); m.clear();
                }
                if(true){
                    ofxOscMessage m;
                    m.setAddress( "/simulation/autoMode" );
                    m.addFloatArg( 0.0 );
                    sender.sendMessage( m ); m.clear();
                    m.setAddress( "/simulation/blackAndWhiteMode" );
                    m.addFloatArg( 0.0 );
                    sender.sendMessage( m ); m.clear();
                }
            }

        }

        void SendEveryTickOSC(){
            /// Where constantly updating information should be organized for sending out.

            if(true)    /// send increasingIndex
            {
                ofxOscMessage m;
                m.setAddress( "/simulation/increasingIndex" );
                m.addFloatArg( ofMap(increasingIndex, 0, increasingIndexMax, 0, 1) );
                sender.sendMessage( m );
            }
        }

        void SendBigCode(){     /// Sends 196 messages, the grid.  Called when page2 and page3 is selected.

            int xIndex = 1; int yIndex = 1;
            for(int i = 0; i < CODE_LENGTH; i++)
            {
                ofxOscMessage m;
                string codeString = "";
                m.setAddress( "/simulation/bigCode/"+ofToString(xIndex)+"/"+ofToString(yIndex));
                m.addFloatArg( float(code[i]) );
                sender.sendMessage( m );

                xIndex++; if(xIndex > 14) { yIndex++; xIndex = 1; }
            }

        }

        void mutateCode(){

            for(int i = 0; i < CODE_LENGTH; i++)
            {
                if(ofRandom(0,1) < MUTATION_RATE)
                {
                    if(code[i] == 0) { code[i] =1; }
                    else{code[i] = 0;}
                }
            }

            SendCodeOSC();

        }

        void doOSC(){

        SendEveryTickOSC();

        // clear strings
        for ( int i=0; i<NUM_MSG_STRINGS; i++ )
        {
            if ( timers[i] < ofGetElapsedTimef() )
                msg_strings[i] = "";
        }

        // check for waiting messages
        while( receiver.hasWaitingMessages() )
        {
            // get the next message
            ofxOscMessage m;
            receiver.getNextMessage( &m );

            if ( m.getAddress() == "/simulation/xy" )
            {
                terminalX = m.getArgAsFloat( 0 ) * ofGetWidth();
                terminalY = m.getArgAsFloat( 1 ) * ofGetHeight();
            }
            else if ( m.getAddress() == "/1")
            {

            }
            else if ( m.getAddress() == "/2")
            {
                SendBigCode();
            }
            else if ( m.getAddress() == "/3")
            {
                if(true){   // operators
                    for(int i = 0; i < 8; i++)
                    {   // send operator switches
                        ofxOscMessage m;
                        m.setAddress( "/simulation/op"+ofToString(i,0) );
                        m.addFloatArg( operatorSwitches[i]);
                        sender.sendMessage( m );
                    }
                }
                SendBigCode();
                CreateAndSendMathString();
            }
            else if ( m.getAddress() == "/simulation/ALLOFF" )
            {
                float val = m.getArgAsFloat( 0 ) ;
                if(val != 0) { bALLOFF = true; } else { bALLOFF = false; }
            }
            else if ( m.getAddress() == "/simulation/alpha" )
            {
                masterAlpha = m.getArgAsFloat( 0 ) ;
            }
            else if ( m.getAddress() == "/simulation/newcode" )
            {
                float val = m.getArgAsFloat( 0 ) ;
                if(val != 0) { bNewCode = true; } else { bNewCode = false; }
            }
            else if ( m.getAddress() == "/simulation/mutate" )
            {
                float val = m.getArgAsFloat( 0 ) ;
                if(val != 0) { bMutateCode = true; } else { bMutateCode = false; }
            }
            else if ( m.getAddress() == "/simulation/increasingIndexPause" )
            {
                float val = m.getArgAsFloat( 0 ) ;
                if(val != 0) { bIncreasingIndexPause = true; } else { bIncreasingIndexPause = false; }
            }
            else if ( m.getAddress() == "/simulation/increasingIndex" )
            {
                increasingIndex = m.getArgAsFloat( 0 ) * increasingIndexMax;
            }
            else if ( m.getAddress() == "/simulation/indexFast")
            {
                if( m.getArgAsFloat(0) == 1.0) { bIncreasingIndexFast = true;  }  else { bIncreasingIndexFast = false; }
            }
            else if ( m.getAddress() == "/simulation/indexSlow")
            {
                if( m.getArgAsFloat(0) == 1.0) { bIncreasingIndexSlow = true;  }  else { bIncreasingIndexSlow = false; }
            }
            else if ( m.getAddress() == "/simulation/resetAll")
            {
                if( m.getArgAsFloat(0) == 1.0 )
                {
                    resetState();
                }
            }
            else if ( m.getAddress() == "/simulation/redLevel" )
            {
                redLevel = m.getArgAsFloat(0);
            }
            else if ( m.getAddress() == "/simulation/greenLevel" )
            {
                greenLevel = m.getArgAsFloat(0);
            }
            else if ( m.getAddress() == "/simulation/blueLevel" )
            {
                blueLevel = m.getArgAsFloat(0);
            }
            else if ( m.getAddress() == "/simulation/blackAndWhiteMode")
            {
                bIsBlackAndWhite = int(m.getArgAsFloat(0));
                CreateAndSendMathString();
            }
            else
            {
                // unrecognized message: display on the bottom of the screen
                string msg_string;
                msg_string = m.getAddress();
                msg_string += ": ";
                for ( int i=0; i<m.getNumArgs(); i++ )
                {
                    // get the argument type
                    msg_string += m.getArgTypeName( i );
                    msg_string += ":";
                    // display the argument - make sure we get the right type
                    if( m.getArgType( i ) == OFXOSC_TYPE_INT32 )
                        msg_string += ofToString( m.getArgAsInt32( i ) );
                    else if( m.getArgType( i ) == OFXOSC_TYPE_FLOAT )
                        msg_string += ofToString( m.getArgAsFloat( i ) );
                    else if( m.getArgType( i ) == OFXOSC_TYPE_STRING )
                        msg_string += m.getArgAsString( i );
                    else
                        msg_string += "unknown";
                }
                //cout << msg_string.c_str() << endl;
                // add to the list of strings to display
                msg_strings[current_msg_string] = msg_string;
                timers[current_msg_string] = ofGetElapsedTimef() + 5.0f;
                current_msg_string = ( current_msg_string + 1 ) % NUM_MSG_STRINGS;
                // clear the next line
                msg_strings[current_msg_string] = "";
            }

                   // get operators
            for(int i = 0; i < 8; i++)
            {
                if ( m.getAddress() == "/simulation/op" +ofToString(i,0) )
                {
                    // the single argument is a string
                    operatorSwitches[i] = int(m.getArgAsFloat( 0 ) );
                    CreateAndSendMathString();

                }
            }

            // /simulation/bigCode/14/12
            int xIndex = 1; int yIndex = 1;
            for(int i = 0; i < CODE_LENGTH; i++)
            {
                if( m.getAddress() == "/simulation/bigCode/"+ofToString(xIndex)+"/"+ofToString(yIndex))
                {
                    code[i] = int(m.getArgAsFloat(0));
                    CreateAndSendMathString();
                }
                xIndex++; if(xIndex > 14) { xIndex = 1; yIndex++;}
            }


        }


        }

        void TickIncreasingIndex(){

            if(bSaverMode)
            {   // save 100 frames of the simulation
                increasingIndex+=0.1;
                if(increasingIndex > increasingIndexMax) { increasingIndex = 0;  bSaverMode = false; }
            }
            else if(!bIncreasingIndexPause)
            {
                if(bIncreasingIndexFast) { increasingIndex+= 0.5; }
                else if(bIncreasingIndexSlow) { increasingIndex+= 0.01; }
                else { increasingIndex += 0.1; }

                if(increasingIndex > increasingIndexMax) { increasingIndex = 0; }   // wrap around
            }

        }

};

