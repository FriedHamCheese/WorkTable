![](https://github.com/FriedHamCheese/WorkTable/blob/code-doc/doc/readme.png)
# What's WorkTable?
WorkTable is a simple assignment list program which uses bars with different length and colors to represent different due dates. It is written in C++ with FLTK as UI library.


# What makes it different / Features
- Bars spanning across the timeline with different lengths and colors to represent a task with a specific due date. It can also have a group of tasks, showing different colors with different lengths representing each tasks in a single bar representing the group as a whole.
- Tasks can also be dragged around to merge with another task or group to form a group, or dragged out of.


# Building the program
## Prerequisites
+ A compiler which supports C++20
+ Make
+ A built FLTK codebase and its prerequisites.
+ Doxygen (optional, documentation for developers)

## Building FLTK
Follow the official building procedures from [FLTK documentation](https://www.fltk.org/doc-1.3/intro.html).

## Building WorkTable
1. Download the source code of the project and extract it to where you would like. Navigate to the extracted directory. We will refer to this directory as "project directory".
2. In the project directory, copy "user_fltk_flags - template" and rename it as "user_fltk_flags".
  In the FLTK directory you have successfully built (or in the project directory if FLTK is installed to your environment. If so, don't type the preceding ./), run
```
./fltk-config --cxxflags
./fltk-config --ldflags
```
  This would output something like  
```
-I/mingw64/include -march=nocona -msahf -mtune=generic -pipe -Wp,-D_FORTIFY_SOURCE=2 -fstack-protector-strong -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 ; from --cxxflags
-L/mingw64/lib -pipe -Wl,-subsystem,windows -mwindows -lfltk -lole32 -lcomctl32 -lws2_32 ; from --ldflags
```
  You would need to copy these to FLTK_CXXFLAGS and FLTK_LDFLAGS in user_fltk_flags respectively. Which would look like this:
```
FLTK_CXXFLAGS := -I/mingw64/include -march=nocona -msahf -mtune=generic -pipe -Wp,-D_FORTIFY_SOURCE=2 -fstack-protector-strong -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64
FLTK_LDFLAGS := -L/mingw64/lib -pipe -Wl,-subsystem,windows -mwindows -lfltk -lole32 -lcomctl32 -lws2_32
```
4. Run Make with no specific target in the project directory.
5. The executable is located in bin as build.exe. Since the compiler and linker is called from the project directory rather than the ./src directory, you should not enter ./bin and run build.exe directly, rather you would run it as ./bin/build.exe from the project directory.
6. (Optional) Developer documentation in html Doxygen.
   Navigate to the doc folder and run `Doxygen docconf` to generate the offline html documentation.
   The landing page of the documentation is in ./doc/html/index.html.

And with that, a warning dialog should pop up as a first boot greet!


# How to use WorkTable
## First boot
Because the tasks.txt file (task file) is not present, the program will notify that it couldn't open the file. It also tells you to not save; this only applies when you have a valid task file with your valuable tasks, but it failed to open the file (for an unknown reason), so you don't wipe your task file clean by saving an empty state. Because we actually don't have the file, we can proceed and save when we made changes to the task file.

![](https://github.com/FriedHamCheese/WorkTable/blob/code-doc/doc/pages/how_to_use/blank_tasks.PNG)

## Creating a new task
Click the paper with folded top right edge to create a new task, the button is in the bottom left. A window should appear.

![](https://github.com/FriedHamCheese/WorkTable/blob/code-doc/doc/pages/how_to_use/empty_taskpropwin.PNG)

Let's create your first task. Any task name is valid, but an open curly bracket at the end of the task name is prohibited. For the due date, we use yyyy/(m)m/(d)d format. The parenthesis are optional digits of month or day. So you can fill in either 05 or just 5 for May. The same applies for days as well, 09 or 9 for the 9th day of the month. You can also mix the month having a leading zero, but the days don't, or the other way around as well :D
I'll create a task for getting groceries like the following:

![](https://github.com/FriedHamCheese/WorkTable/blob/code-doc/doc/pages/how_to_use/taskpropwin_groceries.PNG)

Click create, and a bar should appear. The color and length of the bar depends on how close the task is to its due date. If a task is overdue, it gets moved behind the current date line, indicating the task is overdue.

![](https://github.com/FriedHamCheese/WorkTable/blob/code-doc/doc/pages/how_to_use/first_task.PNG)

## Editing and deleting a task
Click on the task you wish to manipulate. A window will pop up. Clicking delete would delete the task. For editing, you can edit the name and the due date, click Save, and the bar will adjust according to your changes. If you don't want to do either, press ESC to close the window.

![](https://github.com/FriedHamCheese/WorkTable/blob/code-doc/doc/pages/how_to_use/edit_delete_taskpropwin.PNG)

## Saving and discarding your changes
The save button (floppy disk) and the discard button (undo symbol, hemispheric arrow pointing counterclockwise) are in the bottom left corner, right of the new task button.
Clicking the save button saves the current state of your tasks and groups (we will get to this). Clicking the discard button reverts the current state of tasks and groups back to of the task file.

## Changing the timescale
You can narrow down or widen the timescale view with the buttons in the bottom right corner. The button with plus symbol narrows down the timescale (zooms in), but button with minus does the opposite. The lengths of your tasks would change to be relative to the timescale, the color and position will not. The timescale is 1 month by default.
The line on the left represents today, the line on the right represents (timescale) ahead from today.

![](https://github.com/FriedHamCheese/WorkTable/blob/code-doc/doc/pages/how_to_use/timescale_widest.PNG)
Default timescale (1 month)
![](https://github.com/FriedHamCheese/WorkTable/blob/code-doc/doc/pages/how_to_use/timescale_less_wide.PNG)
Narrowed down to 2 weeks

## Groups
Try dragging a task on top of another. A group is made. You can also merge a task with existing group (or the other way), or merge group with another group by drag and drop. 

![](https://github.com/FriedHamCheese/WorkTable/blob/code-doc/doc/pages/how_to_use/new_group.PNG)

Clicking a group will enter "group view", where all the bars are only a single task. So before we clicked and entered the group, we were viewing in "root view", where bars can either be a group or a single task. A button with a straight arrow pointing to the left is now active, this indicates we are in group view and is how we return to root view by clicking it. 
Group view interacts very similarly with root view, the only difference is you cannot merge tasks to get a group; but a box on the left would appear, dragging the task to the box would pop the task out of the group.

![](https://github.com/FriedHamCheese/WorkTable/blob/code-doc/doc/pages/how_to_use/dragging_in_groupview.png)

### Edit or delete a group
To do so, you must be in root view. Right click the group, a window will appear. It is very similar to window of a task, except it doesn't have an input for a specific due date.

**Deleting the group means deleting the tasks in it as well.**

![](https://github.com/FriedHamCheese/WorkTable/blob/code-doc/doc/pages/how_to_use/taskgroupwin.PNG)

That covers all the functionality of WorkTable. Have fun!
