# ft_ls

42 school `ft_ls` project.

---

**Disclaimer: This project was developped for a purely pedagogic purpose. It is not meant to replace your system's ls command.**

---

## Summary

A partial rewriting of [GNU coreutils's ls command](https://www.gnu.org/software/coreutils/manual/html_node/ls-invocation.html#ls-invocation) written in c for linux.

*The objective of this project was to learn the different type of files and their attributes in a filesystem.
Because of the amount of data to display and the option to list subdirectories recursively, a well-thought code structure was primordial.*

---

## Building and usage
### Dependencies
This project requires `Make` and the `gcc` compiler.
### Building
```sh
git clone https://github.com/TheoSignore/ft_ls
cd ft_ls
make
```
### Usage
`./ft_ls [OPTION] ... [FILE] ...`

Options:

`-l`: [long listing format](https://www.gnu.org/software/coreutils/manual/html_node/What-information-is-listed.html#index-_002dl-6) (ACLs and security contexts are not handled)

`-R`: list subdirectories recursively

`-a`: do not ignore entries starting with .

`-r`: reverse order while sorting

`-t`: sort by time, newest first

---

## Description

List information about the FILEs (the current directory by default).

---

Written by [Théo Signore](https://github.com/TheoSignore)
