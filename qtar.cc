#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>
#include <deque>
#include <map>
#include <set>

#include <dirent.h>
#include <err.h>
#include <fcntl.h>
#include <grp.h>
#include <pwd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/types.h>
#include <unistd.h>

#include "linux_getdents.h"
#include "firstblock.h"

static bool use_getdents_type = true;

template<class T>
struct Queue {
	std::deque<T> queue;

	Queue() {
	}

	void push(T item) {
		queue.push_back(item);
	}

	T pop() {
		T ret = queue.front();
		queue.pop_front();
		return ret;
	}

	bool empty() {
		return queue.empty();
	}
};

template<class T>
struct CScan {
	std::set<T> elevator;
	uint64_t last_position;

	CScan() : last_position(0) {
	}

	void push(T item) {
		elevator.insert(item);
	}

	T pop() {
		typename std::set<T>::iterator it;
		T bound;
		bound.d_ino = last_position;

		it = elevator.lower_bound(bound);
		if (it == elevator.end()) {
			if (last_position == 0)
				throw new std::exception();//"pop on empty queue");
			last_position = 0;
			return pop();
		}

		T ret = *it;
		elevator.erase(it);
		last_position = ret.d_ino;
		return ret;
	}

	bool empty() {
		return elevator.empty();
	}
};

template<template <typename> class QueueType>
class Traverser {
	std::string root;

public:
	Traverser() {}

	void walk(std::string& r) {
		root = r;

		struct us_dirent d;
		d.d_ino = -1;
		d.set_is_dir();
		d.parent = root;
		d.d_name = ".";
		d.stat();
		dirheap.push(d);

		while (!dirheap.empty()) {
			d = dirheap.pop();
			heapwalk(d);
		}

		while (!fileheap.empty()) {
			d = fileheap.pop();
			print_file_path(d);
		}
	}

private:

	QueueType<struct us_dirent> dirheap;
	QueueType<struct us_dirent> fileheap;

	void queuefile(struct us_dirent& file) {
		file.d_ino = firstblock(file.fq().c_str());

		fileheap.push(file);
	}

	void print_file_path(struct us_dirent& file) {
		std::string fq = file.fq();
		std::string rel = fq.substr(root.length() + 1);
        std::cout << rel << std::endl;
	}

	void heapwalk(struct us_dirent& dirent) {
		if (dirent.is_file() || dirent.is_link()) {
			queuefile(dirent);
			return;
		}

		std::string path;
		if (dirent.d_name != ".") {
			path = dirent.parent + "/" + dirent.d_name;
		} else  {
			path = dirent.parent;
		}

		if (dirent.is_unknown()) {
			struct stat& buf = dirent.stat();

			if (S_ISDIR(buf.st_mode)) {
				dirent.set_is_dir();
			} else {
				dirent.set_is_file();
				queuefile(dirent);
				return;
			}
		}

		std::vector<struct us_dirent> l;
		if (dir_read(path, l) < 0) {
			std::cerr << "Error!" << std::endl;
			return;
		}

		sort(l.begin(), l.end());

		dirent.set_is_dir();

		for (std::vector<struct us_dirent>::iterator it = l.begin(); it != l.end(); ++it) {
			if (it->d_name == "." || it->d_name == "..")
				continue;

			if (!use_getdents_type)
				it->set_is_unknown();

			if (it->is_file()) {
				queuefile(*it);
			} else {
				dirheap.push(*it);
			}
		}
	}
};

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "usage: sudo qtar /path/to/input-dir\n";
        return 1;
    }

	std::string root = argv[1];

	Traverser<CScan> t;
	t.walk(root);

	return 0;
}
