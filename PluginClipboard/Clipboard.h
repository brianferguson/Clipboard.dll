/*
  Copyright (C) 2012 Brian Ferguson

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef __CLIPBOARD_H__
#define __CLIPBOARD_H__

#include "StdAfx.h"

class CClipboard
{
public:
	// Constructor - Sets initial sequence number
	CClipboard();

	// Set user-defined options
	void SetTextOnly(bool setting = false) { textOnly = setting; }
	void SetErrorString(std::wstring text) { errorString = text; }
	void SetMaxIndex(int max) { maxIndex = max; }

	// Get/Set Windows Clipboard contents
	bool GetClipboard();
	bool SetClipboard(LPCWSTR text);

	// Get/Set current/historical data from clipboard object
	LPCWSTR GetIndex(int index = 0);
	bool SetIndex(std::wstring text, int index = 0);
	void PushBack(std::wstring text) { clipData.push_back(text); }

	// Delete specific index or entire clipboard (ClearClipboard clears Windows Clipboard)
	bool DeleteIndex(int index = 0);
	bool DeleteClipboard();
	bool ClearClipboard();

private:
	// User-defined data
	std::wstring errorString;
	bool textOnly;
	int maxIndex;

	// Current/Historical data of the Windows Clipboard
	std::vector<std::wstring> clipData;

	// Represents the current Windows Clipboard Sequence Number
	// http://msdn.microsoft.com/en-us/library/windows/desktop/ms649042%28v=vs.85%29.aspx
	DWORD sequenceNumber;

	// Used to get initial contents of the clipboard
	bool initialized;
};

#endif //__CLIPBOARD_H__