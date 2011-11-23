#define CODE_LENGTH 196

uniform float time;
uniform vec2 resolution;
uniform vec2 mouse;
uniform int code[CODE_LENGTH];
uniform int operators[8];
uniform float redLevel;
uniform float greenLevel;
uniform float blueLevel;
uniform int bIsBlackAndWhite;

void main(){

    int maxCrawlSize = CODE_LENGTH - 8;
    float positionX = gl_FragCoord.x / mouse.x;
    float positionY = gl_FragCoord.y / mouse.y;

    /// code format
    ///
    /// _0_ _1_ _1_ _1_ _1_ _0_ _0_ _1_
    ///  color |  operator |  terminal

    float runningTally[4];  /// R, G, and B values + constant
    runningTally[0] = runningTally[1] = runningTally[2] = runningTally[3] = 0.0;
    int colorIndex = 0;     // this is changed in the while loop and used to reference which color we are operating on

    bool colorBit0 = false; bool colorBit1 = false;
    bool operatorBit0 = false; bool operatorBit1 = false; bool operatorBit2 = false;
    bool terminalBit0 = false; bool terminalBit1 = false; bool terminalBit2 = false;

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

        // choose the operator
        if(code[codeIndex+0] != 0) { operatorBit0 = true; }
        if(code[codeIndex+1] != 0) { operatorBit1 = true; }
        if(code[codeIndex+2] != 0) { operatorBit2 = true; }
        codeIndex+=3;   // done picking the operator

        /// operator __value__

        if(!operatorBit0 && !operatorBit1 && !operatorBit2 && (operators[0] == 1))                   // 000
        {   /// SIN _runningTally[colorIndex]_
            /// PERFORM THE OPERATION
            runningTally[colorIndex] = sin(runningTally[colorIndex]);
        }
        else if(!operatorBit0 && !operatorBit1 && operatorBit2  && (operators[1] == 1))               // 001
        {   /// COS _runningTally[colorIndex]_
            /// PERFORM THE OPERATION
            runningTally[colorIndex] = cos(runningTally[colorIndex]);
        }
        else if(!operatorBit0 && operatorBit1 && !operatorBit2  && (operators[2] == 1))               // 010
        {   /// fract _runningTally[colorIndex]_
            /// PERFORM THE OPERATION
            runningTally[colorIndex] = fract(runningTally[colorIndex]);
        }
        else if(!operatorBit0 && operatorBit1 && operatorBit2  && (operators[3] == 1))                // 011
        {   /// ADD _chosenValue_
            /// currenValue + (a value from the list from the terminal set)
            float valueToOperateOn = 0.0;
            bool v0 = false; bool v1 = false;
            if(code[codeIndex+0] != 0) { v0 = true; }
            if(code[codeIndex+1] != 0) { v1 = true; }
            codeIndex+=2;
            if(!v0 && !v1) { valueToOperateOn = time; } /// operate on time
            if(!v0 &&  v1) { valueToOperateOn = time; } /// operate on positionX
            if( v0 && !v1) { valueToOperateOn = positionY; } /// operate on positionY
            if( v0 &&  v1) { valueToOperateOn = runningTally[3]; } /// operate on runningTally[colorIndex]
            /// PERFORM THE OPERATION
            runningTally[colorIndex] += valueToOperateOn;
        }
        else if(operatorBit0 && !operatorBit1 && !operatorBit2  && (operators[4] == 1))               // 100
        {   /// SUB _chosenValue_
            /// currenValue + (a value from the list from the terminal set)
            float valueToOperateOn = 0.0;
            bool v0 = false; bool v1 = false;
            if(code[codeIndex+0] != 0) { v0 = true; }
            if(code[codeIndex+1] != 0) { v1 = true; }
            codeIndex+=2;
            if(!v0 && !v1) { valueToOperateOn = time; } /// operate on time
            if(!v0 &&  v1) { valueToOperateOn = positionX; } /// operate on positionX
            if( v0 && !v1) { valueToOperateOn = positionY; } /// operate on positionY
            if( v0 &&  v1) { valueToOperateOn = time; } /// operate on runningTally[colorIndex]
            /// PERFORM THE OPERATION
            runningTally[colorIndex] -= valueToOperateOn;
        }
        else if(operatorBit0 && !operatorBit1 && operatorBit2  && (operators[5] == 1))                // 101
        {   /// MULT _chosenValue_
            /// currenValue + (a value from the list from the terminal set)
            float valueToOperateOn = 0.0;
            bool v0 = false; bool v1 = false;
            if(code[codeIndex+0] != 0) { v0 = true; }
            if(code[codeIndex+1] != 0) { v1 = true; }
            codeIndex+=2;
            if(!v0 && !v1) { valueToOperateOn = time; } /// operate on time
            if(!v0 &&  v1) { valueToOperateOn = positionX; } /// operate on positionX
            if( v0 && !v1) { valueToOperateOn = time; } /// operate on positionY
            if( v0 &&  v1) { valueToOperateOn = runningTally[3]; } /// operate on runningTally[colorIndex]
            /// PERFORM THE OPERATION
            runningTally[colorIndex] *= valueToOperateOn;
        }
        else if(operatorBit0 && operatorBit1 && !operatorBit2  && (operators[6] == 1))               // 110
        {   /// sqrt _runningTally[colorIndex]_
            /// PERFORM THE OPERATION
            runningTally[colorIndex] += sin(0.01*distance(gl_FragCoord.xy, mouse.xy));
        }
        else if(operatorBit0 && operatorBit1 && operatorBit2  && (operators[7] == 1))               // 111
        {   /// fract _runningTally[colorIndex]_
            /// PERFORM THE OPERATION
            //runningTally[colorIndex] = fract(runningTally[colorIndex]);
            runningTally[colorIndex] += cos(0.01*distance(gl_FragCoord.xy, mouse.xy));
        }

    }

	if(bIsBlackAndWhite == 0)  gl_FragColor = vec4(runningTally[0]*redLevel, runningTally[1]*greenLevel, runningTally[2]*blueLevel, 1.0);
	else gl_FragColor = vec4(runningTally[0]);

}
