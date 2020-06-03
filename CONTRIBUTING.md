Contribution Instructions
=========================

This project welcomes contributions. Contributions are managed through GitHub
pull requests.

# Issue guidelines

If you find an issue with any code in this project, feel free to open an issue.
Templates exist for bug reports and feature requests.

# Pull request guidelines

* Fork this repository.
* Create a branch off master, and make your commits in that branch.
* Test your changes locally before raising a pull request.

# Commit guidelines

* Isolate each commit to a single component/folder as far as possible
* Use a standard message template. I follow the template from [this
  link](https://codeinthehole.com/tips/a-useful-template-for-commit-messages/)
* Ensure that you have any necessary tests included to test your changes.

## Additional commit requirements

* Commits should have a `Signed-off-by` line. You can use `git commit -s` to
  automatically append this to your message.
* Any generated objects should be included in `.gitignore`.
* A commit should be self-contained, i.e., if I check out any commit in a clean
  workspace, I should be able to run `./.travis_build.sh` and not encounter any
  failures.
* Any additional dependencies should be called out in `INSTALL.md`

# Contribution License

The project is licensed under GPLv2, with a linking exception. Your
contributions will also be licensed the same way. However, you maintain the
copyright to your contributions.

If this is your first contribution to this project, you may update the `AUTHORS`
file with your full name. Please keep the change to this file in a separate
commit.

