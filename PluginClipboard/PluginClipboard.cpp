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

#include "PluginClipboard.h"

void clearIndex(std::wstring str);

// Contains all parent measures
std::vector<ParentMeasure*> g_ParentMeasures;

PLUGIN_EXPORT void Initialize(void** data, void* rm)
{
	ChildMeasure* child = new ChildMeasure;
	*data = child;

	void* skin = RmGetSkin(rm);

	LPCWSTR parentName = RmReadString(rm, L"ParentName", L"");
	if (!*parentName)
	{
		// Create Parent structure and set its identification values
		child->parent = new ParentMeasure;
		child->parent->name = RmGetMeasureName(rm);
		child->parent->skin = skin;
		child->parent->ownerChild = child;

		// Create clipboard object
		child->parent->clipboard = new CClipboard;

		g_ParentMeasures.push_back(child->parent);
	}
	else
	{
		// Find parent using name AND the skin handle to be sure that it's the right one
		std::vector<ParentMeasure*>::const_iterator iter = g_ParentMeasures.begin();
		for ( ; iter != g_ParentMeasures.end(); ++iter)
		{
			if (_wcsicmp((*iter)->name, parentName) == 0 &&
				(*iter)->skin == skin)
			{
				child->parent = (*iter);
				return;
			}
		}

		RmLog(LOG_ERROR, L"ClipBoard.dll: Invalid ParentName=");
	}
}

PLUGIN_EXPORT void Reload(void* data, void* rm, double* maxValue)
{
	ChildMeasure* child = (ChildMeasure*)data;
	ParentMeasure* parent = child->parent;

	if (!parent)
	{
		return;
	}

	// Read common options (Both Parent and Child can use Index)
	child->index = RmReadInt(rm, L"Index", 0);

	// Read parent specific options
	if (parent->ownerChild == child)
	{
		bool skip = 0!=RmReadInt(rm, L"TextOnly", 0);
		if (!skip)
		{
			parent->clipboard->SetTextOnly(false);
		}
		else
		{
			parent->clipboard->SetTextOnly(true);
		}

		LPCWSTR errorString = RmReadString(rm, L"ErrorString", L"");
		parent->clipboard->SetErrorString(errorString);
	}
}

PLUGIN_EXPORT double Update(void* data)
{
	ChildMeasure* child = (ChildMeasure*)data;
	ParentMeasure* parent = child->parent;

	if (!parent || child->index < 0)
	{
		return 0.0;
	}

	// Only check the clipboard if the parent is updating, not the child.
	if (parent->ownerChild == child)
	{
		parent->clipboard->GetClipboard();
	}

	// Convert the string to a number
	// If the string cannot be converted, 0.0 is returned
	return _wtof(parent->clipboard->GetIndex(child->index));
}

PLUGIN_EXPORT LPCWSTR GetString(void* data)
{
	ChildMeasure* child = (ChildMeasure*)data;
	ParentMeasure* parent = child->parent;

	if (!parent || child->index < 0)
	{
		return L"";
	}

	return parent->clipboard->GetIndex(child->index);
}

PLUGIN_EXPORT void Finalize(void* data)
{
	ChildMeasure* child = (ChildMeasure*)data;
	ParentMeasure* parent = child->parent;

	if (parent && parent->ownerChild == child)
	{
		delete parent;

		// Delete Parent from global list of Parent Measures
		std::vector<ParentMeasure*>::iterator iter = std::find(g_ParentMeasures.begin(), g_ParentMeasures.end(), parent);
		g_ParentMeasures.erase(iter);
	}

	delete child;
}

PLUGIN_EXPORT void ExecuteBang(void* data, LPCWSTR args)
{
	ChildMeasure* child = (ChildMeasure*)data;
	ParentMeasure* parent = child->parent;

	// !CommandMeasure can only be used on the Parent measure.
	if (!parent || parent->ownerChild != child)
		return;

	// !CommandMeasure Clear
	if (_wcsicmp(args, L"Clear") == 0)
	{
		std::wstring currentData = parent->clipboard->GetIndex(0);

		// Clear contents of the Clipboard
		if (!parent->clipboard->DeleteClipboard())
		{
			RmLog(LOG_ERROR, L"Clipboard.dll: Unable to delete clipboard");
		}
		else
		{
			if (!currentData.empty())
			{
				// Clear index 0 of ALL instances (if equal to current index 0)
				clearIndex(currentData);
			}
		}

		return;
	}
	else
	{
		// Check for commands with args.
		LPCWSTR arg = wcschr(args, L' ');
		if (arg)
		{
			// Skip the space
			++arg;

			// !CommandMeasure ClearIndex n (where n represents an index)
			if (_wcsnicmp(args, L"ClearIndex", 10) == 0)
			{
				int index = _wtoi(arg);
				std::wstring currentData = parent->clipboard->GetIndex(index);

				if (!currentData.empty())
				{
					// Index 0 represents the Windows Clipboard. If deleting this index, other parent measure need to be modified.
					if (index == 0)
					{
						parent->clipboard->ClearClipboard();
						clearIndex(currentData);
					}
					else
					{
						if (!parent->clipboard->DeleteIndex(index))
						{
							std::wstring error = L"Clipboard.dll: Unable to delete index \"";
							error += index;
							error += L'"';
							RmLog(LOG_ERROR, error.c_str());
						}
					}
				}

				return;
			}
			// !CommandMeasure "CopyIndex n" (where n represents an index)
			else if (_wcsnicmp(args, L"CopyIndex", 9) == 0)
			{
				LPCWSTR text = parent->clipboard->GetIndex(_wtoi(arg));

				if (!parent->clipboard->SetClipboard(text))
				{
					RmLog(LOG_ERROR, L"Clipboard.dll: Unable to set clipboard");
				}
			}
			else
			{
				RmLog(LOG_ERROR, L"Clipboard.dll: Unknown args");
			}
		}
	}
}

// Helper function to erase index 0 from other Parent Measures
void clearIndex(std::wstring str)
{
	for (auto iter = g_ParentMeasures.begin(); iter != g_ParentMeasures.end(); ++iter)
	{
		std::wstring text = (*iter)->clipboard->GetIndex(0);

		if (str == text)
		{
			(*iter)->clipboard->SetIndex(L"");
		}
		//else
		//{
		//	(*iter)->clipboard->PushBack(L"");
		//}
	}
}