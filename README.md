Clipboard.dll
=============

Clipboard.dll is a plugin for [Rainmeter](http://www.rainmeter.net) that will display the current contents of the [Windows Clipboard](http://en.wikipedia.org/wiki/Clipboard_%28computing%29#Microsoft_Windows).


Contents
-

<ul>
 <li>[Features](https://github.com/brianferguson/Clipboard.dll#features)</li>
 <li>[Options](https://github.com/brianferguson/Clipboard.dll#options)</li>
  <ul>
   <li>[Parent Measures Only](https://github.com/brianferguson/Clipboard.dll#for-parent-measures-only)</li>
    <ul>
     <li>[TextOnly](https://github.com/brianferguson/Clipboard.dll#TextOnly)</li>
     <li>[ErrorString](https://github.com/brianferguson/Clipboard.dll#ErrorString)</li>
     <li>[Example](https://github.com/brianferguson/Clipboard.dll#ParentExample)</li>
    </ul>
   <li>[Child Measures Only](https://github.com/brianferguson/Clipboard.dll#for-child-measure-only)</li>
    <ul>
     <li>[ParentName](https://github.com/brianferguson/Clipboard.dll#ParentName)</li>
     <li>[Example](https://github.com/brianferguson/Clipboard.dll#ChildExample)</li>
    </ul>
   <li>[Both Parent and Child Measures](https://github.com/brianferguson/Clipboard.dll#for-all-clipboard-measures)</li>
    <ul>
	 <li>[Index](https://github.com/brianferguson/Clipboard.dll#Index)</li>
	</ul>
  </ul>
 <li>[Download](https://github.com/brianferguson/Clipboard.dll#download)</li>
  <ul>
   <li>[Using Git](https://github.com/brianferguson/Clipboard.dll#Git)</li>
   <li>[As a .zip](https://github.com/brianferguson/Clipboard.dll#Zip)</li>
   <li>[32-bit Plugin](https://github.com/brianferguson/Clipboard.dll#32)</li>
   <li>[64-bit Plugin](https://github.com/brianferguson/Clipboard.dll#64)</li>
  </ul>
 <li>[Build Instructions](https://github.com/brianferguson/Clipboard.dll#build-instructions)</li>
 <li>Examples</li>
  <ul>
   <li>[Example 1](https://github.com/brianferguson/Clipboard.dll#Example1)</li>
   <li>[Example 2](https://github.com/brianferguson/Clipboard.dll#Example2)</li>
  </ul>
</ul>

 
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

* <a name="TextOnly" class="anchor" href="#TextOnly"><span class="mini-icon mini-icon-link"></span></a>**TextOnly** - When "On", only logs **text** items in the Windows Clipboard. When "Off" and **non-text** items are copied to the Windows Clipboard, then the ErrorString is returned. (`TextOnly=OFF` is default)
* <a name="ErrorString" class="anchor" href="#ErrorString"><span class="mini-icon mini-icon-link"></span></a>**ErrorString** - This is what is returned when **non-text** is copied to the Windows Clipboard. For example, if you copy a file or an image, then ErrorString will be returned. (An empty string is default.)

<a name="ParentExample" class="anchor" href="#ParentExample"><span class="mini-icon mini-icon-link"></span></a>
#####Example:
```ini
[ParentMeasure]
Measure=Plugin
Plugin=Clipboard
TextOnly=OFF
ErrorString=<Invalid data>
```

###For Child measure *only*:

* <a name="ParentName" class="anchor" href="#ParentName"><span class="mini-icon mini-icon-link"></span></a>**ParentName** - The name of the Parent Measure. Do not include brackets. ie. `ParentName=ParentMeasure` is valid. `ParentName=[ParentMeasure]` is invalid.

<a name="ChildExample" class="anchor" href="#ChildExample"><span class="mini-icon mini-icon-link"></span></a>
#####Example:
```ini
[ChildMeasure]
Measure=Plugin
Plugin=Clipboard
ParentName=ParentMeasure
```

###For ALL Clipboard measures:

* <a name="Index" class="anchor" href="#Index"><span class="mini-icon mini-icon-link"></span></a>**Index** - Represents the current content of the Windows Clipboard. Can be used on both Parent and Child measures. (`Index=0` is default)


Download
-
####To download the current source code:

* <a name="Git" class="anchor" href="#Git"><span class="mini-icon mini-icon-link"></span></a>Using git: `git clone git@github.com:brianferguson/Clipboard.dll.git`
* <a name="Zip" class="anchor" href="#Zip"><span class="mini-icon mini-icon-link"></span></a>Download as a [.zip](https://github.com/brianferguson/Clipboard.dll/zipball/master)

####To download current plugin (.dll):

* <a name="32" class="anchor" href="#32"><span class="mini-icon mini-icon-link"></span></a>[32-bit version](https://github.com/brianferguson/Clipboard.dll/blob/master/PluginClipboard/x32/Release/Clipboard.dll)
* <a name="64" class="anchor" href="#64"><span class="mini-icon mini-icon-link"></span></a>[64-bit version](https://github.com/brianferguson/Clipboard.dll/blob/master/PluginClipboard/x64/Release/Clipboard.dll)


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

<a name="Example1" class="anchor" href="#Example1"><span class="mini-icon mini-icon-link"></span></a>
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

<a name="Example2" class="anchor" href="#Example2"><span class="mini-icon mini-icon-link"></span></a>
####Example 2:
This example will save the last 3 **text only** items copied to the Windows Clipboard. `ErrorString` has no meaning when using the option `TextOnly`.

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