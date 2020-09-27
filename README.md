# BMX Bike Robot Team Repo

## Get you Computer set up:

1. Follow these [instructions](https://github.com/dlynch7/Tiva_Make#tiva_make) to get all of the stuff you need to build the source code. This is a separate directory from the actual project folder.

2. Create a new directory on you computer to hold the project code. I called mine `bmx_bike`.

3. Set up your git stuff:

  ```
  cd ~/bmx_bike # or whatever you called it
  git init
  git remote add upstream https://github.com/NU-BMX-Bike-Robot/bmx-bike.git  
  ```

  This repository should be considered master. So the code in here should always be the most functional, up-to-date code.

4. Next create your development repository. Go the the [team repository](https://github.com/NU-BMX-Bike-Robot/bmx-bike.git) and create a fork. This will be your development area and should show up on your personal github page. After you have created the fork run the following commands:

  ```
  cd ~/bmx_bike # or whatever you called it
  git remote add origin https://github.com/<your-user-name-here>/bmx-bike.git
  ```

  If setup the same way as noted above, you can use `git push upstream master` to send changes to the team project repository and `git push origin master` to send changes to your personal development repository.

5. Edit the makefile to match the locations for everything installed in step 1.

 - I have added the Makefile to the .gitignore list as not to overwrite for people developing on different OS. If you make any required changes to that please inform everyone.


6. Grab one of the dev boards we have and test if you've set things up properly. You should be able to complete everything on Dan's page up until the `make flash` and `make screen` commands at the very bottom.



## How to use Git for this project

This is an outline for the general process we should all follow when starting to develop a new feature. [If you have never used git before here is a lot of good info and some examples to help](http://robotics.mech.northwestern.edu/~elwin/git_intro.html)

First, make sure your local master branch is up to date with the upstream remote master branch on github.com
```
git fetch upstream
git merge
```

Now your local files have the most up to date functioning code and you can start developing. If someone pushes an update to upstream while you are doing your development, follow the same process above and resolve any merge conflicts.

Be sure to commit often during your development but only push to your development repository.
```
git add <any new files or directories go here>
git commit -a # type a commit message at the top of your text editor
git push origin master
```

Once you have finished and verified your development is functional, you are ready to push to upstream. **Before making a commit to upstream, ensure you don't have any unmerged changes and reach out to team members so we don't overwrite any progress.** It's fixable, just a headache :)

```
git fetch upstream
git merge

# Do a final check to make sure you will not be overwriting stuff on master

git push upstream master
```
