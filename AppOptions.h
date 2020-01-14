/*
		Author: Mikhail Lyubushkin

		Project:		Move Your Files 2.0
		Description:	Application option
*/

#pragma once

struct AppOptions
{
public:
	void ReadOptions();
	void WriteOptions() const;

public:
	bool bURAS = false;
	bool bDNSDWIF = false;
	bool bMI = true;
	bool bDNSDWEF = false;
	bool bAI = true;
};

