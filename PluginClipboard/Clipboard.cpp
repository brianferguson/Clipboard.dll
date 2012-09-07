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

#include "Clipboard.h"

/*
** Clipboard class constructor
**  - Sets initial sequence number.
*/
CClipboard::CClipboard() :
	errorString(),
	textOnly(false),
	initialized(false),
	clipData()
{
	sequenceNumber = GetClipboardSequenceNumber();
}

/*
** Gets the current contents of the clipboard
**
** Return:
**  If true, the Windows Clipboard's contents are gathered. If the contents are non-text, the text
**    in "errorString" is copied into the vector ONLY if the user-defined "textOnly" value is false.
**  If false:
**    1. The Windows Clipboard could not be opened. (Very rare)
**    2. The Windows Clipboard could not be closed. (Very rare)
*/
bool CClipboard::GetClipboard()
{
	// Get current sequence number
	// Sequence number changes if new data is copied to the clipboard (or if the clipboard is emptied)
	DWORD seqNum = GetClipboardSequenceNumber();

	// Needs to run on first update whether sequence number has changed or not
	if (!initialized || seqNum != sequenceNumber)
	{
		sequenceNumber = seqNum;

		if (OpenClipboard(NULL))
		{
			// Check for text
			// CF_UNICODETEXT converts to CF_TEXT and CF_OEMTEXT (and vice-versa)
			// http://msdn.microsoft.com/en-us/library/windows/desktop/ms649013%28v=vs.85%29.aspx#_win32_Synthesized_Clipboard_Formats
			if (IsClipboardFormatAvailable(CF_UNICODETEXT))
			{
				std::wstring currentData = (LPCWSTR)GetClipboardData(CF_UNICODETEXT);

				// Do not push data back if the current data is empty
				// The current data is ONLY empty when another Parent measure
				//  clears the contents of the clipboard
				if (!clipData.empty() && clipData.back() == L"")
				{
					clipData[clipData.size() - 1] = currentData;
				}
				else
				{
					clipData.push_back(currentData);
				}
			}
			// Non-text is in the clipboard AND clipboard is NOT empty
			// Apparently an empty clipboard still has a format, so count needs to be above 2
			else if (!textOnly && CountClipboardFormats() > 2)
			{
				clipData.push_back(errorString);
			}

			initialized = true;
			return CloseClipboard();
		}
	}

	return false;
}

/*
** Sets "text" to the clipboard
**
** Return:
**  If true, "text" is copied into the Windows Clipboard. Likewise,
**   all Parent Measures will contain this "text" on the next update cycle.
**  If false:
**   1. "text" is empty. (Most likely cause)
**   2. The Windows Clipboard could not be emptied. (Very rare)
**   3. The Windows Clipboard could not be opened. (Very rare)
**   4. The Windows Clipboard could not be closed. (Very rare)
*/
bool CClipboard::SetClipboard(LPCWSTR text)
{
	size_t length = wcslen(text);

	if (length > 0)
	{
		const size_t size = (length * 2) + 1;
		HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, size);

		memcpy(GlobalLock(hMem), text, size);
		GlobalUnlock(hMem);

		if (OpenClipboard(NULL))
		{
			// Clear current contents of Windows Clipboard
			if (!EmptyClipboard())
			{
				return false;
			}
			
			// Set "text" to the Windows Clipboard
			SetClipboardData(CF_UNICODETEXT, hMem);
			return CloseClipboard();
		}
	}

	return false;
}

/*
** Retrieves an index from the vector
**  - Remember that index 0 is at the end of the vector, not the first.
**
** Return:
**  The string at "index" is return (if valid). If "index" is invalid, and empty string is returned.
*/
LPCWSTR CClipboard::GetIndex(int index)
{
	if (clipData.empty())
	{
		return L"";
	}

	int maxIndex = clipData.size() - 1;

	// reset "index" to start from the back of the vector
	index = maxIndex - index;

	if (!(index < 0) && index <= maxIndex)
	{
		return clipData[index].c_str();
	}

	return L"";
}

/*
** Sets text to an index in the vector
**  - This is a special function used when other Parent Measures clear Index 0 either
**    from "!CommandMeasure Clear" or "!CommandMeasure ClearIndex 0".
**  - Remember that index 0 is at the end of the vector, not the first.
**
** Return:
**  If true, "text" is set to the correct "index" in the vector. ("text" will usually equal and empty string)
**  If false:
**   1. The vector is empty. (The most likely cause)
**   2. The index is out of range. (Very rare since we only use this for index 0)
*/
bool CClipboard::SetIndex(std::wstring text, int index)
{
	if (clipData.empty())
	{
		return false;
	}

	int maxIndex = (int)clipData.size() - 1;

	// reset "index" to start from the back of the vector
	index = maxIndex - index;

	if (index >= 0 && index <= maxIndex)
	{
		clipData[index] = text;
		return true;
	}

	return false;
}

/*
** Deletes a specific index
**  - All indices below "index" in the stack will get moved up
**  - Remember that index 0 is at the end of the vector, not the first.
**
** Return:
**  If true:
**   1. If "index" is 0, then the data stored in "index" is emptied, but not erased.
**   2. If "index" is not 0, then the data in "index" is erased from the vector AND
**      all the data in indices after "index" gets moved down one position in the vector.
**  If false:
**   1. If "index" is 0, then the clipboard could not be cleared. (Very rare)
**   2. The clipboard is empty. (The most likely cause)
*/
bool CClipboard::DeleteIndex(int index)
{
	if (!clipData.empty())
	{
		if (index == 0)
		{
			// We don't want items moving into Index 0
			clipData.back() = L"";
			return ClearClipboard();
		}

		// .erase will move the content of the other indices after "index" down one position
		clipData.erase(clipData.end() - index - 1);
		return true;
	}

	return false;
}

/*
** Deletes entire the clipboard
**  - This also deletes the contents in the Windows Clipboard
**
** Return:
**  If true, the vector is cleared AND the Windows Clipboard is cleared.
**  If false, the Windows Clipboard could not be cleared. (Very rare)
*/
bool CClipboard::DeleteClipboard()
{
	clipData.clear();
	return ClearClipboard();
}

/*
** Clears the contents of the Windows Clipboard (regardless if it text or not)
**
** Return:
**  If true, the Windows Clipboard is cleared.
**  If false, the Windows Clipboard could not be cleared. (Very rare)
*/
bool CClipboard::ClearClipboard()
{
	if (OpenClipboard(NULL))
	{
		if (!EmptyClipboard())
		{
			return false;
		}

		return CloseClipboard();
	}
	
	return false;
}