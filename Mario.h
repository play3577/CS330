/*
 *  Mario.h
 *  Mario
 *
 *  Created by Andrew Peddicord on 2/1/11.
 *  Copyright 2011 Capital University. All rights reserved.
 *
 */
//Object constant will be 6
#ifndef __MARIO_H__
#define __MARIO_H__
const int SMALL_STATE = 0, BIG_STATE = 1, FIRE_STATE = 2;

#include "AllMovable.h"

class Mario: public AllMovable {
    
public:
	//Constructor
	Mario();
  
	virtual void draw();
  	//Gets called when key is pressed
	void updateKeyDown(unsigned char button);
	//Gets called when key is let up
	void updateKeyUp(unsigned char button);
	//Returns State of Mario
	inline int getState() {return state_;}
    
    //updates Mario 
    void updateScene();
    
    //returns the object type of Mario
    virtual int objectType() { return 6; }
    
    static void loadTexture();

private:
	//State can be 0,1,2 depending if he is Big/Little/Fire Mario
	int state_, jumpCount_, starCount_; 
	bool isInvincible_;
    bool jumpKey_, rightKey_, leftKey_, sprintKey_, fireballKey_;
    
	//Moves Mario to the left or right and calls check() at beginning
	void move();
    //checks to see if Mario runs into an object
    bool check();
	//Returns true if Mario is in the fire state
	bool fireball();
    
    //Handels all jump cases
    void jump();
	
    static GLuint textureNum;
};

#endif
