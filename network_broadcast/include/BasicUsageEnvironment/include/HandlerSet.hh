/**********
This library is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the
Free Software Foundation; either version 2.1 of the License, or (at your
option) any later version. (See <http://www.gnu.org/copyleft/lesser.html>.)

This library is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
more details.

You should have received a copy of the GNU Lesser General Public License
along with this library; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
**********/
// Copyright (c) 1996-2010 Live Networks, Inc.  All rights reserved.
// Basic Usage Environment: for a simple, non-scripted, console application
// C++ header
/*
Handler是什么呢？它是一种专门用于执行socket操作的任务（函数），HandlerSet被
TaskScheduler用来管理所有的socket任务（增删改查）．所以TaskScheduler中现在已
调度两种任务了：socket任务（handlerSet）和延迟任务(DelayQueue)．其实
TaskScheduler还调度第三种任务：Event
*/

#ifndef _HANDLER_SET_HH
#define _HANDLER_SET_HH

#ifndef _BOOLEAN_HH
#include "Boolean.hh"
#endif

////////// HandlerSet (etc.) definition //////////
//handler结构
class HandlerDescriptor {
  HandlerDescriptor(HandlerDescriptor* nextHandler);
  virtual ~HandlerDescriptor();

public:
  //几个主要参数
  int socketNum;
  int conditionSet;
  TaskScheduler::BackgroundHandlerProc* handlerProc;
  void* clientData;

private:
  // Descriptors are linked together in a doubly-linked list:
	//双向链表
  friend class HandlerSet;
  friend class HandlerIterator;
  HandlerDescriptor* fNextHandler;
  HandlerDescriptor* fPrevHandler;
};

class HandlerSet {
public:
  HandlerSet();
  virtual ~HandlerSet();

  void assignHandler(int socketNum, int conditionSet, TaskScheduler::BackgroundHandlerProc* handlerProc, void* clientData);
  void clearHandler(int socketNum);
  void moveHandler(int oldSocketNum, int newSocketNum);

private:
  HandlerDescriptor* lookupHandler(int socketNum);

private:
  friend class HandlerIterator;
  HandlerDescriptor fHandlers;
};

class HandlerIterator {
public:
  HandlerIterator(HandlerSet& handlerSet);
  virtual ~HandlerIterator();

  HandlerDescriptor* next(); // returns NULL if none
  void reset();

private:
  HandlerSet& fOurSet;
  HandlerDescriptor* fNextPtr;
};

#endif
