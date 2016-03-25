# Basic git commands

This is a short getting started guide providing only bare essentials that we need for the project.
The following assumes that you have installed git command-line interface on your computer. There's also a GUI available, but this tutorial assumes you are using command line. To install git on Windows refer to [this guide](https://git-scm.com/book/en/v2/Getting-Started-Installing-Git#Installing-on-Windows). On UNIX systems you should be able to do this with one line command.
Also you should have an account on [GitHub](github.com) to be able to add code to the repo.

Feel free to try out the following commands in order they are presented. This will get you acquainted with the process.


## Cloning a repo

Setting up your local repository (where all the sources will be located) and downloading current verison of the repository is called cloning.
You can do this by the following command:

```
git clone https://github.com/raunc/bat-detector
```

This will clone the repo to local `bat-detector` sub-folder.
To carry out any operations on the repo (like adding code), you should navigate to this folder `cd bat-detector`.


## Using branches

If you are developing a new feature then it is good idea to do this on a different branch. This way your half-finished changes won't mess up the working code.

To list the existing branches, run the following:

```
git branch -a
```

List of branches should be listed on screen, and the current active branch is marked with asterisk (*) in front of it.
The main branch is usually called `master` and should contain only code that is working.


The simplest way to create a new branch and start using it is the following (replace `new-branch-name` with anything you like):

```
git checkout -b new-branch-name
```

Run `git branch -a` again and see the what's changed. There should be a new branch that is active.

To move between branches use `checkout` command without `-b` flag, e.g:

```
git checkout master
```

If you run `git branch -a` again, you should see that active branch is changed.


## Adding your files

Move back to you newly created branch with `git checkout new-branch-name` and then add a text file into folder. You can use any text editor you like, just ensure that the file gets saved in the git repo folder (the folder that was created with `clone` command before).

Now run the following to see the changes:

```
git status
```

This should print out your current branch and show the changed files in red.

To add your changes for committing (more on commits below), run the following:

```
git add .
```

This will add all changes made to files. `.` stands for current directory and everything, including sub-folders, gets added. To add single files, specify space separated filenames instead of `.`.

`git status` should now show the changes in green. This means they are ready to be committed. You can think of commits as packs of changes applied to code.

To create a commit, run:

```
git commit -m "Message describing the changes"
```

Message should always be short and descriptive, e.g "Fixed adc sample interval". Never something like "some changes".

Now the commit can be pushed (read: uploaded) to the GitHub for others to use.
As this is the first push, we must also set up the same branch in GitHub. Run:

```
git push -u origin new-branch-name
```

Later, when you have already pushed some changes for your branch, you can just run:

```
git push
```


### Summary

To add you changes you need 3 commands:

```
git add .
git commit -m "Message"
git push
```


## Keeping your files sync with GitHub

Git does not sync the files automatically, so you could decide which files get included in commits.
So to get the work others have done, you need to pull (read: download) files from GitHub to your computer.

Pull files is simple:

```
git pull
```


## Resolving conflicts

Sometimes when you try to `git push` your commits, git refuses to do this. This is usually, because someone else has also edited something and your local copy of the code is out of sync.
So you need to `git pull` in order to get the latest changes. This however, can lead to conflicts. When two people have edited the same lines in the same files,
then git does not know which of the changes are correct and you must edit the files manually to resolve this. The situation is called merge conflict.

It could be sometimes tricky to resolve the conflicts, but don't panic. It is totally expected situation from time to time. I'm not going to explain the resolving process here, just ask help if you can't handle it yourself.

## Adding changes to master

**Important: if you try the following for testing purposes, don't use master for that, but create another new branch**

When you have finished working with a new feature on a branch and you are confident that it doesn't break anything, it is time to put the changes to master branch. There are few steps for doing this:

1) sync master
2) rebasing (optional, but creates a cleaner commit history and allows to resolve conflicts before anything gets to master)
3) merging
4) pushing
5) delete feature branch (optional, but you probably don't need this branch if changes are already in master)

### Sync master

To sync code on master, do the following:

```
git checkout master
git pull
git checkout your-branch
```

This is just going to master branch, pulling all updates and going back to your feature branch.

### Rebasing

To rebase your commits, do the following while on the branch that will be merged to master:

```
git rebase master
```

For advanced users:

```
git rebase -i master
```

The second version with `-i` flag allows to concatenate commits, remove commits or edit commit messages.
The first version just takes your commits and tries to put them on top of the master commits. The result is basically the same as you would have started working with the code that is currently on the master branch.

**Beware of conflicts in this step.** They occur if your changes overlap with some changes on master. There's no escape from it, if skip this step, then conflicts just appear later in the merge phase.

### Merge and push

Now that your code is rebased, you can safely merge it to master branch and publish your changes.

```
git checkout master
git merge your-branch
git push
```

### Delete feature branch

The following commands will remove the branch from your local repo and GitHub. Be careful with this.

```
git branch -D your-branch
git push origin --delete your-branch
```


## Summary

That should be enough for decently working with the git repo. If you want deeper understanding of branches, commits, commit tree structure etc, just google it. There's plenty (even too much) of resources available.
