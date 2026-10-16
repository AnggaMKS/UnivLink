# UnivLink Demo Video Guide

This guide provides a recommended order for recording a UnivLink demonstration.
The sequence is designed to show the application workflow, the three user
roles, the academic features, persistence, and the four OOP pillars without
making the video unnecessarily long.

## 1. Recommended video structure

| Section | Suggested duration | What to demonstrate |
|---|---:|---|
| Introduction | 30–45 seconds | Project purpose, technology, and main roles |
| Build and startup | 30–45 seconds | Build the application and explain the first screen |
| Admin workflow | 2–3 minutes | Users, curriculum, rooms, classes, and schedules |
| Lecturer workflow | 2–3 minutes | Announcements, assignments, submissions, attendance, and grades |
| Student workflow | 2–3 minutes | Dashboard, KRS, schedule, assignments, and transcript |
| Persistence | 30–45 seconds | Save, restart, and reload data |
| OOP explanation | 2–3 minutes | Encapsulation, abstraction, inheritance, and polymorphism |
| Closing | 20–30 seconds | Summarize the result and architecture |

The full demonstration should take approximately 10–14 minutes. If a shorter
video is required, keep the Admin, Lecturer, and Student sections and shorten
the OOP explanation to one example per pillar.

## 2. Preparation before recording

Perform these steps before starting the recording:

1. Open a terminal in the project root.
2. Confirm that the project has this structure:

   ```text
   app/
   cli/
   akademik/
   storage/
   data/
   README.md
   Makefile
   ```

3. Build the application:

   ```powershell
   mingw32-make clean
   mingw32-make
   ```

   On Linux or Git Bash, use:

   ```bash
   make clean
   make
   ```

4. For a fresh demo, make sure `data/univlink.dat` and
   `data/univlink.bak` do not contain an old session. Move them to a backup
   location or delete them only if the existing demo data is not needed.
   The seed data is created only when no users exist in the loaded data.
5. Run the application from the repository root so the relative `data/`
   directory is found:

   ```powershell
   .\univlink.exe
   ```

6. Use the following seeded accounts:

   | Role | ID | Password |
   |---|---|---|
   | Admin | `A001` | `admin123` |
   | Lecturer | `D001` | `dosen123` |
   | Lecturer | `D002` | `dosen123` |
   | Student | `M001` | `mhs12345` |
   | Student | `M002` | `mhs12345` |
   | Student | `M003` | `mhs12345` |

## 3. Section 1: Introduction

Start the recording by explaining:

> UnivLink is a C++17 university academic management application. It
> supports administrators, lecturers, and students. The system manages
> curricula, courses, classes, rooms, schedules, KRS registration,
> assignments, attendance, grades, transcripts, and file persistence.

Briefly show the project folders:

- `app/`: application entry point and seed data.
- `cli/`: terminal input, menus, and role-specific interaction.
- `akademik/`: academic domain classes and business rules.
- `storage/`: file persistence implementation.
- `data/`: saved runtime data.

Mention that the application uses a layered structure:

```text
CLI -> SistemAkademik -> academic domain
                    -> IPenyimpanan / PenyimpananFile
```

## 4. Section 2: Build and startup

Show the build command and briefly explain the important compiler options:

- `-std=c++17`: compile using C++17.
- `-Wall -Wextra -Wpedantic`: enable compiler warnings.
- `-O2`: enable optimization.
- `-I.`: allow headers to be included from the project root.

Run the executable and show the initial menu:

```text
1. Masuk
2. Daftar akun mahasiswa
0. Keluar
```

Explain that `main.cpp` loads saved data first. If no users are found,
`seed.cpp` creates the sample users, courses, rooms, classes, schedules,
historical grades, and sample assignments.

## 5. Section 3: Admin demonstration

Log in with:

```text
ID       : A001
Password : admin123
```

### 5.1 Show the Admin dashboard

Use **Lihat Dashboard** and point out:

- Number of students.
- Number of lecturers.
- Number of administrators.
- Number of courses.
- Number of rooms.
- Number of classes.
- Warning for classes without rooms or schedules.

Explain that the dashboard reads information from `SistemAkademik`, which
aggregates the main domain objects.

### 5.2 Show user management

Open **Kelola Pengguna** and show the list of users. Explain that the system
supports the three role types:

- `Admin`
- `Dosen`
- `Mahasiswa`

If you create a new student during the recording, use a temporary demo
identity and explain that registration is validated before insertion.

### 5.3 Show curriculum and courses

Open **Kelola Mata Kuliah & Kurikulum** and demonstrate:

1. View the existing courses.
2. View the curriculum contents.
3. Show that courses are grouped by semester.
4. Point out the distinction between theory and practical courses.
5. Show prerequisites if the menu option is available.

Explain that practical courses use laboratory rooms, while theory courses use
classrooms. The course type is represented by the subclasses
`MataKuliahTeori` and `MataKuliahPraktikum`.

### 5.4 Show room management

Open **Kelola Ruangan** and show the seeded rooms:

- Classroom rooms such as `R101`–`R110`.
- Laboratory rooms such as `LAB1`, `LAB2`, and `LAB3`.

Explain that a room has a capacity and a room type. The room type must match
the course requirement.

### 5.5 Show class and schedule management

Open **Kelola Kelas** and demonstrate:

1. List existing classes.
2. Open a class detail.
3. Show the course, lecturer, period, room, schedule, and capacity.
4. If creating a temporary class, use **Buat kelas baru**.
5. Show **Tempatkan ruangan & jadwal**.

Attempt to choose an incompatible room only if you want to demonstrate
validation. For example, do not assign a classroom to a practical course.
The system should reject the operation because of room-type validation.

Also mention that schedule placement validates:

- Valid start and end times.
- Room availability.
- Room capacity.
- Lecturer schedule conflicts.
- Student schedule conflicts.

Log out after the Admin section.

## 6. Section 4: Lecturer demonstration

Log in with:

```text
ID       : D001
Password : dosen123
```

### 6.1 Show the Lecturer dashboard

Use **Lihat Dashboard** and show the classes assigned to the lecturer.
Explain that a lecturer can only operate on classes that they teach.

### 6.2 Create and publish an announcement

Open the announcement menu and:

1. Select a class.
2. Choose **Buat pengumuman baru**.
3. Enter a title and message.
4. Edit the announcement if desired.
5. Choose **Terbitkan pengumuman**.

Explain the announcement lifecycle:

```text
DRAFT -> TERBIT
```

After publication, log in as a student later and show that the announcement
appears in the student announcement area.

### 6.3 Create an assignment

Open the assignment menu and:

1. Select a class.
2. Choose **Buat tugas baru**.
3. Enter a title and description.
4. Set a future deadline.
5. Set an assignment weight.

Important: the deadline must be in the future. Use a date far enough ahead
that it will remain valid during recording.

Explain that assignments belong to a class and can have multiple student
submissions.

### 6.4 View and score submissions

Use the seeded student profile `M001` or `M002` because they already contain
sample assignment activity:

- `M001`: sample assignments are submitted and scored.
- `M002`: only half of the sample assignments are submitted.

Open **Lihat pengumpulan** to show the submission status. Then use **Beri
skor tugas** to assign a score to an existing submission.

Point out that the lecturer cannot score an assignment that the student has
not submitted. This demonstrates domain-level validation.

### 6.5 Record attendance

Open the attendance menu and:

1. Select a class.
2. Choose **Catat kehadiran satu pertemuan**.
3. Select a meeting number.
4. Mark a student as present or absent.
5. Show **Rekap kehadiran**.

Explain that attendance contributes to the automatic `kehadiran` grade
component during grade finalization.

### 6.6 Input and finalize grades

Open the grade menu and demonstrate the recommended order:

1. Open **Rekap nilai**.
2. Choose **Input nilai per komponen**.
3. Enter components such as `uts` and `uas`.
4. Choose **Finalisasi nilai**.
5. Show the resulting grade distribution.

Explain that:

- Scores are validated between 0 and 100.
- `tugas` and `kehadiran` are calculated automatically.
- Finalization calculates the final score and letter grade.
- Finalized grades are locked and cannot be changed normally.

This is an important feature to show because it demonstrates encapsulation and
business rules inside the academic classes.

Log out after the Lecturer section.

## 7. Section 5: Student demonstration

Use `M001` for the most complete student demonstration:

```text
ID       : M001
Password : mhs12345
```

### 7.1 Show the Student dashboard

Choose **Lihat Dashboard** and point out:

- Student identity.
- Entry year.
- Current semester.
- IPK.
- Latest IPS.
- Maximum allowed SKS.
- Total passed SKS.
- Current semester schedule.
- Active announcements.

Explain that the displayed IPK and IPS are calculated by `Transkrip`, not
entered manually.

### 7.2 Show the transcript

Choose **Lihat Transkrip Nilai** and show:

- Course code and name.
- SKS.
- Letter grade.
- Grade weight.
- Academic period.
- Class grade distribution.
- IPS.
- IPK.
- Passed SKS.

Mention that `M001` is seeded as a semester-3 student with an example IPK of
3.26 and semester-2 IPS of 3.02.

### 7.3 Show current KRS

Choose **Registrasi Mata Kuliah (KRS)** and demonstrate:

1. View available classes.
2. Select a class.
3. Add it to the KRS.
4. Show the total SKS.
5. View the KRS list.
6. Confirm or **Sahkan KRS**.

Explain that KRS validation checks:

- Course prerequisites.
- Duplicate courses.
- Existing passed courses.
- Maximum SKS.
- Available seats.
- Room and schedule assignment.
- Schedule conflicts.

After the KRS is approved, explain that it can no longer be freely modified.

### 7.4 Show class details and assignments

Choose **Detail Kelas & Portal Tugas** and show:

- Lecturer.
- Room.
- Schedule.
- Attendance percentage.
- Assignment score.
- Grade components.
- Assignment list.
- Submission status.

For a live submission demonstration:

1. Select an assignment that has not been submitted.
2. Choose **Kumpulkan tugas**.
3. Enter a sample file path such as `tugas-demo.pdf`.
4. Show the updated submission status.

If using the seeded data, use `M002` to demonstrate the difference between
submitted and unsubmitted assignments.

### 7.5 Show announcements

Choose **Lihat Pengumuman** and show the announcement created by the lecturer.
This demonstrates the flow from lecturer publication to student visibility.

### 7.6 Optional: demonstrate a first-semester student

Log out and log in with:

```text
ID       : M003
Password : mhs12345
```

Show that `M003` is a first-semester student without historical grades. This
contrasts with `M001` and demonstrates that the dashboard and transcript are
data-dependent.

## 8. Section 6: Persistence demonstration

To demonstrate file persistence:

1. Create or modify a small piece of data, such as an announcement or KRS.
2. Log out.
3. Exit the application.
4. Show that `data/univlink.dat` and `data/univlink.bak` exist.
5. Restart the application.
6. Log in again and show that the data remains available.

Explain that:

- `SistemAkademik` delegates saving and loading to `IPenyimpanan`.
- `PenyimpananFile` implements file-based persistence.
- The application writes a backup to help protect existing data.
- Existing data is loaded before seed data is considered.

This last point is important: changing `app/seed.cpp` does not replace
existing users and records in `data/univlink.dat`.

## 9. Section 7: OOP explanation for the presentation

Use the source tree or `akademik/univlink.h` while explaining the four pillars.
The following examples are short enough for a demo video.

### Encapsulation

Use `KRS` and `Nilai` as examples:

- Their internal state is private.
- `KRS::tambahKelas()`, `KRS::validasi()`, and `KRS::sahkan()` control KRS
  changes.
- `Nilai::updateKomponen()` validates score changes.
- `Nilai::finalisasi()` calculates and locks the grade.

Connect this to the demo by showing that invalid enrollment, invalid scores,
and changes after finalization are rejected.

### Abstraction

Use persistence and transcript calculation:

- The application uses `IPenyimpanan` through `muat()` and `simpan()`.
- `SistemAkademik` does not need to know the file-record format.
- `Transkrip::hitungIPS()` and `Transkrip::hitungIPK()` hide the grade
  calculation details.

Connect this to the demo by showing that the user only chooses a menu option
to view a transcript without seeing the calculation algorithm.

### Inheritance

Show the genuine hierarchies:

```text
User
├── Mahasiswa
├── Dosen
└── Admin

MataKuliah
├── MataKuliahTeori
└── MataKuliahPraktikum
```

Explain that the subclasses represent specialized versions of their base
classes, not unrelated classes forced into a hierarchy.

### Polymorphism

Use two examples:

1. `User*` can refer to an `Admin`, `Dosen`, or `Mahasiswa`. Calling
   `tampilkanDashboard()` or `tampilkanMenu()` produces role-specific output.
2. `MataKuliah*` can refer to a theory or practical course. The actual object
   determines the required room type and grading calculation.

Also mention the overloaded functions:

```cpp
Kurikulum::cariMataKuliah(const string& kode)
Kurikulum::cariMataKuliah(int semester)
```

## 10. Suggested narration for the closing

End with a short summary:

> UnivLink demonstrates a complete academic workflow from administration and
> class scheduling to lecturer assessment and student registration. The
> project applies encapsulation through validated domain methods, abstraction
> through interfaces and high-level operations, inheritance through its user
> and course hierarchies, and polymorphism through role-specific and
> course-specific behavior. Data is persisted to files so the application can
> continue from a previous session.

## 11. Recording checklist

Before submitting the video, confirm that it includes:

- [ ] Project purpose and architecture.
- [ ] Successful build with C++17.
- [ ] Application startup screen.
- [ ] Admin login and dashboard.
- [ ] Course, curriculum, room, class, and schedule features.
- [ ] Lecturer login and dashboard.
- [ ] Announcement creation and publication.
- [ ] Assignment creation and scoring.
- [ ] Attendance recording.
- [ ] Grade input and finalization.
- [ ] Student login and dashboard.
- [ ] KRS registration and validation.
- [ ] Assignment submission.
- [ ] Transcript, IPK, IPS, and passed SKS.
- [ ] Persistence after restarting.
- [ ] Encapsulation explanation.
- [ ] Abstraction explanation.
- [ ] Inheritance explanation.
- [ ] Polymorphism explanation.

