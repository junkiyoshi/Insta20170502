#include "ofApp.h"

//--------------------------------------------------------------
ofApp::~ofApp() {
	for (ofxBulletSphere* tmp : this->obs) { delete tmp; }
	this->obs.clear();

	for (ofxBulletBox* tmp : this->boxes) { delete tmp; }
	this->boxes.clear();
}

//--------------------------------------------------------------
void ofApp::setup(){
	ofSetFrameRate(30);
	ofEnableDepthTest();
	ofBackground(255);
	ofSetWindowTitle("box shot!");

	this->world.setup();
	this->world.setGravity(ofVec3f(0.0, -1024.0, 0));
	world.enableCollisionEvents();
	ofAddListener(world.COLLISION_EVENT, this, &ofApp::onCollision);

	for (int x = -1; x < 2; x++) {
		for (int y = -1; y < 2; y++) {
			ofxBulletSphere* obstacle = new ofxBulletSphere();
			obstacle->create(this->world.world, ofVec3f(x * 300, y * 300, -300), 0.f, 100);
			obstacle->setProperties(0.8, 0.5);
			obstacle->add();

			this->obs.push_back(obstacle);
			this->obs_color.push_back(ofColor(128));
		}
	}
}

//--------------------------------------------------------------
void ofApp::update(){
	this->world.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
	this->cam.begin();

	for (int i = 0; i < this->obs.size(); i++) {
		ofSetColor(this->obs_color[i]);
		this->obs[i]->draw();
	}

	for (int i = 0; i < this->boxes.size(); i++) {
		ofSetColor(this->box_colors[i]);
		this->boxes[i]->draw();
	}
	
	Leap::Frame frame = leap.frame();
	Leap::HandList hands = frame.hands();
	for (int i = 0; i < hands.count(); i++) {
		float r = 0;
		float g = 0;
		float b = ofMap(hands[i].pinchStrength(), 0, 1, 0.0f, 255.0f);

		ofSetColor(r, g, b, 255);

		this->drawHand(hands[i]);

		if (hands[i].grabStrength() < 0.7) {
			ofxBulletBox* box = new ofxBulletBox();
			float size = ofRandom(10.0, 20.0);
			box->create(this->world.world, ofVec3f(hands[i].palmPosition().x, hands[i].palmPosition().y - ofGetHeight() / 3, hands[i].palmPosition().z), 1.0, size, size, size);
			box->setRestitution(1.0);
			//box->applyCentralForce(this->cam.getPosition() * -100);
			box->applyCentralForce(ofVec3f(hands[i].direction().x, hands[i].direction().y, hands[i].direction().z) * 1024 * 64);
			cout << " Z = " << hands[i].palmPosition().z << endl;
			box->add();

			this->boxes.push_back(box);

			ofColor color;
			color.setHsb(ofRandom(255), 255, 255);
			this->box_colors.push_back(color);
		}
	}

	this->cam.end();
}

//--------------------------------------------------------------
void ofApp::drawHand(Leap::Hand hand) {
	Leap::FingerList fingers = hand.fingers();
	for (int j = 0; j < fingers.count(); j++) {
		this->drawFinger(fingers[j]);
	}

	ofPushMatrix();
	ofVec3f palm_point = ofVec3f(hand.palmPosition().x, hand.palmPosition().y - ofGetHeight() / 3, hand.palmPosition().z);
	ofTranslate(palm_point);
	ofSphere(10);
	ofPopMatrix();
}

//--------------------------------------------------------------
void ofApp::drawFinger(Leap::Finger finger) {

	ofVec3f tip_point = ofVec3f(finger.tipPosition().x, finger.tipPosition().y - ofGetHeight() / 3, finger.tipPosition().z);
	ofPushMatrix();
	ofTranslate(tip_point);
	ofSphere(5);
	ofPopMatrix();

	ofVec3f base_point = ofVec3f(tip_point.x + finger.direction().x * finger.length(),// * -1,
		tip_point.y + finger.direction().y * finger.length() - 1,
		tip_point.z + finger.direction().z * finger.length() - 1);
	ofPushMatrix();
	ofTranslate(base_point);
	ofSphere(5);
	ofPopMatrix();

	ofLine(tip_point, base_point);
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

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

//--------------------------------------------------------------
void ofApp::onCollision(ofxBulletCollisionData& cdata) {

	int obs_index = -1;
	int box_index = -1;

	for (int i = 0; i < this->obs.size(); i++) {
		if (*this->obs[i] == cdata) {
			cout << "Collision Obstractions : " << i << " | ";
			obs_index = i;
		}
	}

	for (int i = 0; i < this->boxes.size(); i++) {
		if (*this->boxes[i] == cdata) {
			cout << "Collision Spheres : " << i << " | ";
			box_index = i;
		}
	}

	if (obs_index != -1 && box_index != -1) {
		this->obs_color[obs_index] = this->box_colors[box_index];
	}

	cout << endl;
}