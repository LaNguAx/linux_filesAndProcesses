# Linux Files & Processes Toolkit

Two small POSIX‑C programs that exercise low‑level file‑IO, forking, and process control:

* **`comp`** – a byte‑wise comparator that returns structured exit codes  
  * `2` → files identical `1` → mismatch negative → error  
* **`autograder`** – batch runner that discovers student folders, launches their executables with test inputs, diff‑checks the output via `comp`, and writes results to `grades.csv`.

---

## ✨ What’s inside?

| File / Function              | Purpose                                                  |
|------------------------------|----------------------------------------------------------|
| `comp/main.c`                | Reads two paths, streams both with `read()`/`write()`, exits 2/1/‑1 |
| `autograder/main.c`          | Parses `config.txt`, `fork/execvp`s each student, pipes IO, captures exit status |
| `loadLinesToBuffer()`        | Slurps newline‑delimited files into a dynamic array      |
| `handleError()`              | Minimal perror/exit wrapper for sys‑calls                |
| `students_list.txt` (temp)   | Generated via `ls`, holds discovered student folders     |
| `grades.csv`                 | Final grade report (100 or 0) per student                |

---

## 📝 Customising

* **Config file** – edit `config.txt` to point at the student directory, input file, and expected output file.  
* **Grading logic** – tweak the `grades[]` mapping near the end of `autograder/main.c` to apply partial credit or different weightings.  
* **Capacity** – adjust `MAX_STUDENTS` / `MAX_STUDENT_NAME` constants for larger cohorts.

---

## ⚖️ License

MIT — use, modify, and share freely.
