#pragma once 
#ifndef _ISCORELISTENER_H_
#define _ISCORELISTENER_H_

class IScoreListener
{
public:
	virtual void OnScoreChanged(int score) = 0;
};

#endif
