// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once


#include <iostream>
#include <tchar.h>

#include <stdio.h>

class cpuid_t
{
public:
	//class constructor, copy constructor and destructor
	cpuid_t();
	cpuid_t(const cpuid_t& myCPUID);
	~cpuid_t();


    unsigned long id0;
    unsigned long id1;
    unsigned long id2;
    unsigned long id3;

	cpuid_t getCPUID(unsigned long func_idx);
	bool& operator==(const cpuid_t& myCPUID);

	cpuid_t& operator=(const cpuid_t& myCPUID);

	bool initialize();

private:
	static int cpuID0;
	static int cpuID1;
	static int cpuID2;
	static int cpuID3;

};

//constructor
cpuid_t::cpuid_t()
{
}

//copy constructor
cpuid_t::cpuid_t(const cpuid_t& myCPUID)
{
	this->id0 = myCPUID.id0;
	this->id1 = myCPUID.id1;
	this->id2 = myCPUID.id2;
	this->id3 = myCPUID.id3;
}


bool cpuid_t::initialize()
{
	if((this->id0 == this->cpuID0) && (this->id1 == this->cpuID1) && (this->id2 == this->cpuID2) && (this->id3 == this->cpuID3))
	{
		return true;
	}
	else
	{
		return false;
	}
}



//this overloaded operator determines wether or not the id numbers from the
//local comp match the hard wired private cpuid members
bool& cpuid_t::operator==(const cpuid_t& myCPUID)
{
	bool IO;
	if((myCPUID.id0 == this->cpuID0)||(myCPUID.id1 == this->cpuID1)||(myCPUID.id2 == this->cpuID2)||(myCPUID.id3 == this->cpuID3))
	{
		IO = true;
		return IO;
	}
	else
	{
		IO = false;
		return IO;
	}
}

cpuid_t& cpuid_t::operator=(const cpuid_t& myCPUID)
{
	this->id0 = myCPUID.id0;
	this->id1 = myCPUID.id1;
	this->id2 = myCPUID.id2;
	this->id3 = myCPUID.id3;

	return *this;
}


//destructor
cpuid_t::~cpuid_t()
{
}

//gets the cpu id numbers
cpuid_t cpuid_t::getCPUID(unsigned long func_idx)
{
    cpuid_t id;
    __asm{ 
        mov eax, func_idx
        cpuid
        mov id.id0, eax
        mov id.id1, ebx
        mov id.id2, ecx
        mov id.id3, edx
    }
    return id;
}


//this is the cpuid for the designated computer
int cpuid_t::cpuID0 = 0;//0
int cpuid_t::cpuID1 = 0;//0
int cpuid_t::cpuID2 = 1;//1
int cpuid_t::cpuID3 = 537919488;//537919488

// TODO: reference additional headers your program requires here
