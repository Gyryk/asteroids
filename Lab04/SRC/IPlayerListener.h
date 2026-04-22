#pragma once
#ifndef _IPLAYERLISTENER_H_
#define _IPLAYERLISTENER_H

class IPlayerListener
{
public:
	virtual void OnPlayerKilled(int lives_left) = 0;
};

#endif