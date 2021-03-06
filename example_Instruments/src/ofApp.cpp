#include "ofApp.h"

#define NUMBER_OF_KEYS 10

//--------------------------------------------------------------
void ofApp::setup(){

    /*
     Generators and ControlGenerators both output a steady stream of data.
     Generators output at the sample rate (in this case, 44100 hertz.
     ControlGenerators output at the control rate, which is much lower than the sample rate.
     */
    
    // create a named parameter on the synth which we can set at runtime
    ControlGenerator midiNote = synth.addParameter("midiNumber");
    
    // convert a midi note to a frequency (plugging that parameter into another object)
    ControlGenerator noteFreq =  ControlMidiToFreq().input(midiNote);
    
    // Here's the actual noise-making object
    Generator tone = SawtoothWave().freq( noteFreq );
    
    // Let's put a filter on the tone
    tone = LPF24().input(tone).Q(10).cutoff((noteFreq * 2) + SineWave().freq(1) * 0.5 * noteFreq);
    
    // It's just a steady tone until we modulate the amplitude with an envelope
    ControlGenerator envelopeTrigger = synth.addParameter("trigger");
    Generator toneWithEnvelope = tone * ADSR().attack(0.0).decay(0.5).sustain(0).release(0).trigger(envelopeTrigger).legato(true);
    
    // let's send the tone through some delay
    Generator toneWithDelay = StereoDelay(0.5, 0.75).input(toneWithEnvelope).wetLevel(0.1).feedback(0.2);
    
    synth.setOutputGen( toneWithEnvelope );
    
    
    auto _devices = soundStream.getDeviceList();
    ofSoundStreamSettings _settings;
    if (!_devices.empty()) {
        _settings.setOutDevice(_devices[1]);
    }
    _settings.setOutListener(this);
    _settings.bufferSize = 512;
    _settings.sampleRate = 44100;
    _settings.numInputChannels = 0;
    _settings.numOutputChannels = 2;
    soundStream.setup(_settings);
    
}


//--------------------------------------------------------------
void ofApp::trigger(){
    static int twoOctavePentatonicScale[10] = {0, 2, 4, 7, 9, 12, 14, 16, 19, 21};
    int degreeToTrigger = floor(ofClamp(scaleDegree, 0, 9));
    
    // set a parameter that we created when we defined the synth
    synth.setParameter("midiNumber", 44 + twoOctavePentatonicScale[degreeToTrigger]);
    
    // simply setting the value of a parameter causes that parameter to send a "trigger" message to any
    // using them as triggers
    synth.setParameter("trigger", 1);
}


//--------------------------------------------------------------
void ofApp::setScaleDegreeBasedOnMouseX(){
    int newScaleDegree = ofGetMouseX() * NUMBER_OF_KEYS / ofGetWindowWidth();
    if(ofGetMousePressed() && ( newScaleDegree != scaleDegree )){
        scaleDegree = newScaleDegree;
        trigger();
    }else{
        scaleDegree = newScaleDegree;
    }
}

//--------------------------------------------------------------
void ofApp::update(){
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    float keyWidth = ofGetWindowWidth() / NUMBER_OF_KEYS;
    for(int i = 0; i < NUMBER_OF_KEYS; i++){
        if((i == scaleDegree) && ofGetMousePressed() ){
            ofSetColor(255, 151, 0);
        }else{
            int brightness =  100 + (55 * i / NUMBER_OF_KEYS);
            ofSetColor(brightness, brightness, brightness);
        }
        ofDrawRectangle(keyWidth * i, 0, keyWidth, ofGetWindowHeight());
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    setScaleDegreeBasedOnMouseX();
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    setScaleDegreeBasedOnMouseX();
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
    trigger();
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
    
}

void ofApp::audioRequested (float * output, int bufferSize, int nChannels){
    synth.fillBufferOfFloats(output, bufferSize, nChannels);
}
