Clipboard.dll
=============

Clipboard.dll is a plugin for [Rainmeter](http://www.rainmeter.net) that will display the current contents of the [Windows Clipboard](http://en.wikipedia.org/wiki/Clipboard_%28computing%29#Microsoft_Windows).


Contents
-

* [Features](https://github.com/brianferguson/Clipboard.dll#features)
* [Options](https://github.com/brianferguson/Clipboard.dll#options)
* [Commands]((https://github.com/brianferguson/Clipboard.dll#commands)
* [Download](https://github.com/brianferguson/Clipboard.dll#download)
* [Build Instructions](https://github.com/brianferguson/Clipboard.dll#build-instructions)
* [Examples](https://github.com/brianferguson/Clipboard.dll#examples)
 
 
Features
-
Here are some of the features of the Clipboard plugin:

* Uses the Parent/Child structure.
* Once loaded, the plugin "remembers" any text that is copied to the Windows Clipboard.
* Once new text is copied to the clipboard, the plugin pushes everything back 1 spot. So `Index=0` will always<sup>1</sup> represent the current contents of the clipboard.
* The string is converted to a number (if applicable).

######<sup>1</sup> _**If `TextOnly=ON` then `Index=0` will not equal the current contents of the Windows Clipboard.**_


Options
-
###For Parent measures *only*:

* **TextOnly** - When "On", only logs **text** items in the Windows Clipboard. When "Off" and **non-text** items are copied to the Windows Clipboard, then the ErrorString is returned. (`TextOnly=OFF` is default)
* **ErrorString** - This is what is returned when **non-text** is copied to the Windows Clipboard. For example, if you copy a file or an image, then ErrorString will be returned. (An empty string is default.)

#####Example:
```ini
[ParentMeasure]
Measure=Plugin
Plugin=Clipboard
TextOnly=OFF
ErrorString=<Invalid data>
```

###For Child measure *only*:

* **ParentName** - The name of the Parent Measure. Do not include brackets. ie. `ParentName=ParentMeasure` is valid. `ParentName=[ParentMeasure]` is invalid.

#####Example:
```ini
[ChildMeasure]
Measure=Plugin
Plugin=Clipboard
ParentName=ParentMeasure
```

###For ALL Clipboard measures:

* **Index** - Represents the current content of the Windows Clipboard. Can be used on both Parent and Child measures. (`Index=0` is default)


Commands
-
Commands **only** work on [Parent Measures](https://github.com/brianferguson/Clipboard.dll/blob/master/README.md#for-parent-measures-only).

* **Clear** - Clears the contents of the Windows Clipboard and clears all indices of the Parent measure. Also, `Index=0` of other Parent measures will be deleted only if `Index=0` equals the same on both Parents.
Usage: `!CommandMeasure ParentMeasure Clear`
* **ClearIndex _n_** - Clears index "_n_" and moves other indices to a lower index. If _n_=0, the Windows Clipboard is cleared, and **only** `Index=0` (not all indices) - It also will delete `Index=0` from other Parent measures if the data is the same.
Usage: `!CommandMeasure ParentMeasure "ClearIndex 2"`
* **Copy _string_** - Copies *string* to the Windows Clipboard.
Usage: `!CommandMeasure ParentMeasure "Copy Test"`
* **CopyIndex _n_** - Copies string at index *n* to the Windows Clipboard.
Usage: `!CommandMeasure ParentMeasure "CopyIndex 2"`

Download
-
####To download the current source code:

* Using git: `git clone git@github.com:brianferguson/Clipboard.dll.git`
* Download as a [.zip](https://github.com/brianferguson/Clipboard.dll/zipball/master)

####To download current plugin (.dll):

* [32-bit version](https://github.com/brianferguson/Clipboard.dll/blob/master/PluginClipboard/x32/Release/Clipboard.dll?raw=true)
* [64-bit version](https://github.com/brianferguson/Clipboard.dll/blob/master/PluginClipboard/x64/Release/Clipboard.dll?raw=true)


Build Instructions
-
This plugin was written in c++ using the [Rainmeter Plugin SDK](https://github.com/rainmeter/rainmeter-plugin-sdk)

To build this source code, you will need Visual Studio 2010. If you are using any _**paid**_ version of Visual Studio (Professional, Premium, Ultimate, etc.) you can open "PluginClipboard.sln" and build from there. If you are using Visual C++ 2010 Express, then please install the following **in order**:

1. Visual Studio 2010 Express Edition
2. Visual Studio 2010 SP1
3. Windows 7.1 SDK
4. KB2519277 compiler update

Then you can open "PluginClipboard.sln" and build from there.


Examples
-

####Example 1:
This example will save the last 3 items copied to the Windows Clipboard. Since "MeasureParent" doesn't have a `TextOnly` option, it is turned off, meaning any non-text that is copied to the Windows Clipboard will return `<Invalid Data>`.

```ini
[MeasureParent]
Measure=Plugin
Plugin=Clipboard
ErrorString=<Invalid Data>
;Index=0

[MeasureChild1]
Measure=Plugin
Plugin=Clipboard
ParentName=MeasureParent
Index=1

[MeasureChild2]
Measure=Plugin
Plugin=Clipboard
ParentName=MeasureParent
Index=2
```

####Example 2:
This example will save the last 3 **text-only** items copied to the Windows Clipboard. `ErrorString` has no meaning when using the option `TextOnly`.

```ini
[MeasureParent]
Measure=Plugin
Plugin=Clipboard
TextOnly=ON
;Index=0

[MeasureChild1]
Measure=Plugin
Plugin=Clipboard
ParentName=MeasureParent
Index=1

[MeasureChild2]
Measure=Plugin
Plugin=Clipboard
ParentName=MeasureParent
Index=2
```