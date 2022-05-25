/*

Name - Khushi Dave
Enrolment No - BT20CSE031
BooK Access Management
Data Structure Used - Linked List

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


struct Date
{
    int yyyy;
    int mm;
    int dd;
};
typedef struct Date Date;


int isLeap(int year) // year is leap or common
{
    if (year % 4 == 0 && year % 100 != 0 || year % 400 == 0)
        return 1;
    else
        return 0;
}


int offsetDays(int d, int m, int y)
{
    int offset = d;

    switch (m - 1)
    {
    case 11:
        offset += 30;
    case 10:
        offset += 31;
    case 9:
        offset += 30;
    case 8:
        offset += 31;
    case 7:
        offset += 31;
    case 6:
        offset += 30;
    case 5:
        offset += 31;
    case 4:
        offset += 30;
    case 3:
        offset += 31;
    case 2:
        offset += 28;
    case 1:
        offset += 31;
    }

    if (isLeap(y) && m > 2)
        offset += 1;

    return offset;
}


// Given a year and days elapsed in it, finds
// date by storing results in d and m.
void revoffsetDays(int offset, int y, int *d, int *m)
{
    int month[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    if (isLeap(y))
        month[2] = 29;

    int i;
    for (i = 1; i <= 12; i++)
    {
        if (offset <= month[i])
            break;
        offset = offset - month[i];
    }

    *d = offset;
    *m = i;
}


// Add x days to the given date.
void addDays(Date *d, int x)
{
    if (x == -1 && d->dd == 1 && d->mm == 1)
    {
        d->dd = 31;
        d->mm = 12;
        d->yyyy -= 1;
        return;
    }
    int offset1 = offsetDays(d->dd, d->mm, d->yyyy);
    int remDays = isLeap(d->yyyy) ? (366 - offset1) : (365 - offset1);

    // y2 is going to store result year and
    // offset2 is going to store offset days
    // in result year.
    int y2, offset2;
    if (x <= remDays)
    {
        y2 = d->yyyy;
        offset2 = offset1 + x;
    }

    else
    {
        // x may store thousands of days.
        // We find correct year and offset
        // in the year.
        remDays = x - remDays;
        y2 = d->yyyy + 1;
        int y2days = isLeap(y2) ? 366 : 365;
        while (remDays >= y2days)
        {
            y2days = x - y2days;
            y2++;
            y2days = isLeap(y2) ? 366 : 365;
        }
        offset2 = x;
    }

    // Find values of day and month from
    // offset of result year.
    int m2, d2;
    revoffsetDays(offset2, y2, &d2, &m2);

    d->yyyy = y2;
    d->dd = d2;
    d->mm = m2;
}


/*
structure definition of book_list
1 title of book
2 author of book
3 subject of book
4 number of copies issued of that book
5 number of copies available of that book
6 number of request that were made by student but book was not alloted as there were no copies available
*/

typedef struct book_tag
{
    char title[15];
    char author[15];
    char subject[10];
    int num_copies_issued;
    int num_copies_avail;
    int num_request;
    struct book_tag *next;

} book;


/*
Structure definition for request queue containing students
1 name of students
2 title of book borrowed
3 req_date is the date on which request of book was made by student

*/


typedef struct student_tag
{
    char name[10];
    char title_book_asked[15];
    Date req_date;
    struct student_tag *next;
} student;


/*
Structure definition of borrower list
1 name of borrower
2 title of book borrowed
3 num of books borrowed by that student
4 date is date of returning that book i.e 15 days after allot date
5 req_date is date on which request of book was made

*/

typedef struct borrower_tag
{
    char name[10];
    char title_book_borrowed[15];
    int num_books;
    Date date;
    Date req_date;
    struct borrower_tag *next;

} borrower;



book *book_list = NULL;
student *student_list = NULL;
borrower *borrower_list = NULL;
borrower *defaulter_list = NULL;


// function used for implementing book_list by adding book nodes
void addBooks()
{
    book *b = (book *)malloc(sizeof(book));
    b->next = NULL;
    printf("\nEnter book's title, author, subject, numberof copies available: \n");
    scanf("%s", b->title);
    scanf("%s", b->author);
    scanf("%s", b->subject);
    scanf("%d", &b->num_copies_avail);

    b->num_copies_issued = 0;
    b->num_request = 0;

    int flag = 0;
    if (book_list == NULL)
    {
        flag = 1;
    }
    b->next = book_list;
    book_list = b;

    if (flag)
    {
        book_list->next = NULL; // for pointing first book_list node's next to NULL
    }
}


student *last1_ptr = NULL; // pointer to last node in request separating students those who have borrowed book 1st time or not borrowed any book
student *head1_ptr = NULL; // pointer to first node separating student s who have already borrowed book and adding them at end of list
student *last_ptr = NULL;  // pointer to last node of request queue from which student who already borrowed books are appended


void request_queue()
{
    student *s = (student *)malloc(sizeof(student));
    s->next = NULL;
    printf("Enter name of student and title of book to borrow\n");
    scanf("%s", s->name);
    scanf("%s", s->title_book_asked);
    printf("enter dates of request \n");
    scanf("%d %d %d", &s->req_date.dd, &s->req_date.mm, &s->req_date.yyyy);

    borrower *b = borrower_list;
    borrower *d = defaulter_list;

    // finding whether student is in defaulter list
    // if yes then book may not be alloted to him/her.
    while (d != NULL)
    {
        if (strcmp(d->name, s->name) == 0)
        {
            printf("sorry you have not return %s book before %d %d %d,So book cannot be alloted\n", d->title_book_borrowed, d->date.dd, d->date.mm, d->date.yyyy);
            return;
        }
        d = d->next;
    }

    // finding whether student has borrowed 3 books if yes then book can not be alloted unless 1 is return
    borrower *b_temp = borrower_list;
    int count = 1;
    while (b_temp != NULL)
    {
        if (strcmp(b_temp->name, s->name) == 0)
        {
            count++;
        }
        if (count > 3)
        {
            printf("Sorry you have already borrowed 3 books\n");
            return;
        }
        b_temp = b_temp->next;
    }

    // traversing borrower list to know whther student has borrowed book if yes then append at last of request queue
    while (b != NULL && strcmp(b->name, s->name) != 0)
    {
        b = b->next;
    }

    // traversing request queue to know whether student is in request queue if yes then append at last of request queue
    student *s_ptr = student_list;
    while (s_ptr != last1_ptr && strcmp(s_ptr->name, s->name) != 0)
    {

        s_ptr = s_ptr->next;
    }

    // student has borrowed book and asking for again are appended at end accordingly
    if (b != NULL && strcmp(b->name, s->name) == 0 || s_ptr != NULL && strcmp(s_ptr->name, s->name) == 0)
    {

        // if request_queue is null then this node must be first node of it
        if (last1_ptr == NULL && head1_ptr == NULL && student_list == NULL)
        {
            s->next = student_list;
            student_list = s;
            last1_ptr = s;
            head1_ptr = s;
        }

        // if request queue is not empty but headptr is null means list has no lower priority student so at last this node has to be appended
        else if (last_ptr != NULL && last1_ptr == NULL && head1_ptr == NULL)
        {
            last_ptr->next = s;
            head1_ptr = s;
            last1_ptr = s;
            last1_ptr->next = NULL;
        }

        // else last1_ptr is pointer pointing to last node of lower priority students append there at last
        else
        {

            last1_ptr->next = s;
            last1_ptr = s;
            last1_ptr->next = NULL;
        }
    }
    // student has not borrowed book so higher priority will be appended before lower priority students i.e before head1_ptr i.e pointer pointing to lower priority students
    else
    {
        if (last_ptr == NULL)
        {
            s->next = student_list;
            last_ptr = s;
            last_ptr->next = head1_ptr;
            student_list = s;
            student_list->next = NULL;
        }

        else
        {
            s->next = head1_ptr;
            last_ptr->next = s;
            last_ptr = s;
        }
    }
}


// function for displaying the request queue
void display_list()
{
    if (student_list != NULL)
    {
        student *s = student_list;
        printf("-------------------------Printing Request Queue-------------------------------\n\n");
        while (s != NULL)
        {
            printf("student's name is: %s \n", s->name);
            printf("Title of book asked is: %s \n", s->title_book_asked);
            printf("Request was made on date: %d %d %d \n\n", s->req_date.dd, s->req_date.mm, s->req_date.yyyy);
            s = s->next;
        }
    }
    else
    {
        printf("student list i.e request queue is empty \n\n");
    }
}


// function displaying book list
void display_book_list()
{
    if (book_list != NULL)
    {
        printf("-------------------------Printing Book List-------------------------------\n\n");
        book *s = book_list;
        while (s != NULL)
        {
            printf("Book's title: %s \n", s->title);
            printf("Book's author is: %s \n", s->author);
            printf("Book's subject: %s \n", s->subject);
            printf("Num of Copies issued: %d \n", s->num_copies_issued);
            printf("Num of Copies available: %d \n", s->num_copies_avail);
            printf("Num of request made that were pending to fulfil: %d \n\n", s->num_request);

            s = s->next;
        }
    }
    else
    {
        printf("Book list is empty \n\n");
    }
}


// function displaying borrower list
void display_borrower_list()
{
    if (borrower_list != NULL)
    {
        borrower *s = borrower_list;
        printf("-------------------------Printing Borrower List-------------------------------\n\n");
        while (s != NULL)
        {
            printf("Student's Name is: %s \n", s->name);
            printf("Student Borrowed Book of Title: %s \n", s->title_book_borrowed);
            printf("Num of book Borrowed by student: %d \n", s->num_books);
            printf("Date of request: %d %d %d\n", s->req_date.dd, s->req_date.mm, s->req_date.yyyy);
            printf("Date of returning book is: %d %d %d \n\n", s->date.dd, s->date.mm, s->date.yyyy);
            s = s->next;
        }
    }
    else
    {
        printf("Borrower list is empty \n\n");
    }
}


// function displaying defaulter list
void display_defaulter_list()
{
    if (defaulter_list != NULL)
    {
        borrower *s = defaulter_list;
        printf("-------------------------Printing Defaulter List-------------------------------\n\n"); 
        while (s != NULL)
        {
            printf("Student's name is: %s \n", s->name);
            printf("Title of Borrowed book is: %s \n", s->title_book_borrowed);
            printf("Num of books borrowed is: %d \n", s->num_books);
            printf("Date of return was: %d %d %d \n\n", s->date.dd, s->date.mm, s->date.yyyy);
            s = s->next;
        }
    }
    else
    {
        printf("defaulter list is empty \n\n");
    }
}


// function for updating borrower list i.e adding nodes borrower list those who were alloted books
// while also updating num of books borrowed by students and appending similar named student together
// for ease of sorting ( according to num of book borrowed ) as merge sort is a stable sort

void update_borrower_list(borrower *b)
{
    borrower *b_ptr = borrower_list;
    borrower *b_prev = NULL;
    // finding similar student in borrower list and appending at back of them while some other named student appears
    while (b_ptr != NULL && b_ptr->next != NULL && strcmp(b->name, b_ptr->name) != 0)
    {
        b_prev = b_ptr;
        b_ptr = b_ptr->next;
    }

    // counting number of similar named student and updating num of borrowed books acordingly
    int count = 1;
    borrower *ptr = b_ptr;
    while (b_ptr != NULL && strcmp(b->name, b_ptr->name) == 0)
    {
        b_prev = b_ptr;
        b_ptr = b_ptr->next;
        count++;
    }

    while (ptr != NULL && strcmp(b->name, ptr->name) == 0)
    {
        ptr->num_books = count;
        ptr = ptr->next;
    }

    // appending the node of new borrower with updated num of books borrowed in request queue along with other similar student by using prev pointer
    if (b_prev == NULL)
    {
        b->next = borrower_list;
        borrower_list = b;
        b->num_books = count;
    }
    else
    {
        b_prev->next = b;
        b->next = b_ptr;
        b_ptr = b;
        b->num_books = count;
    }
}


// once request is fulfilled student node is removed from request node and from that data borrower student node is made available
// borrower node is passed to update borrower function for appending it

void remove_student_from_queue(student *s)
{
    borrower *b = (borrower *)malloc(sizeof(borrower));
    b->num_books = 1;

    printf("Enter todays date on which you are allocating book to %s which made request on %d %d %d \n", s->name, s->req_date.dd, s->req_date.mm, s->req_date.yyyy);
    scanf("%d %d %d", &b->date.dd, &b->date.mm, &b->date.yyyy);
    addDays(&b->date, 15); // date of return book

    b->req_date.dd = s->req_date.dd;
    b->req_date.mm = s->req_date.mm;
    b->req_date.yyyy = s->req_date.yyyy;

    strcpy(b->title_book_borrowed, s->title_book_asked);
    strcpy(b->name, s->name);

    // if student is head or first node of request queue
    if (student_list == s)
    {
        student_list = student_list->next;
        if (head1_ptr == last1_ptr && last1_ptr == s)
        { // lower priority nodes are all going to delete hence head1_ptr and last1_ptr points to null
            head1_ptr = NULL;
            last1_ptr = NULL;
        }
        if (head1_ptr == s)
        {
            head1_ptr = s->next; // moving head1_ptr to point to next node as previously pointed node is going to delete
        }
        if (last_ptr == s)
        {
            last_ptr = NULL; // higher priority nodes all going to delete hence pointing last_ptr to NULL
        }
        free(s);
    }
    else
    { // else if node to delete is not first node or head prev pointer wil be considered
        student *s_ptr = student_list;
        student *s_prev = NULL;
        while (s_ptr != s && s_ptr != NULL)
        {
            s_prev = s_ptr;
            s_ptr = s_ptr->next;
        }

        // similar manipulations as done earlier
        if (head1_ptr == last1_ptr && last1_ptr == s_ptr)
        {
            head1_ptr = NULL;
            last1_ptr = NULL;
        }
        if (last_ptr == s_ptr)
        {
            last_ptr = NULL;
        }
        if (head1_ptr == s_ptr)
        {
            head1_ptr = s_ptr->next;
        }
        if (last_ptr == s_ptr && s_prev != NULL)
        {
            last_ptr = s_prev; // if last_ptr is going to delete point it to prevoius node
        }

        // freeing node accordingly
        if (s_ptr == s)
        {
            if (s_prev != NULL && s_ptr->next != NULL)
            {
                s_prev->next = s_ptr->next;
                if (head1_ptr == s_ptr)
                {
                    head1_ptr = s_ptr->next;
                }
            }
            else if (s_prev != NULL && s_ptr->next == NULL)
            {
                s_prev->next = NULL;
            }
            else
            {
                if (student_list != NULL)
                {
                    student_list = student_list->next;
                }
                else
                {
                    student_list = NULL;
                }
            }
            free(s_ptr);
        }
    }
    update_borrower_list(b); // passing b to update in borrower list
}


// alloting book as per date and checking whether requested book is available then requested student node will be alloted book by deleting in request queue and appending in borrower list
// if copy is not available then request is registered

void alloc_book()
{
    student *temp = student_list;
    book *b;
    student *prev;
    while (temp)
    {
        b = book_list;
        while (b->next != NULL && strcmp(temp->title_book_asked, b->title) != 0)
        {
            b = b->next;
        }

        // if book is found in list
        if (strcmp(b->title, temp->title_book_asked) == 0)
        {
            if (b->num_copies_avail > 0)
            { // and available
                b->num_copies_issued++;
                b->num_copies_avail--;
                remove_student_from_queue(temp);
                if (b->num_request > 0)
                {
                    b->num_request--;
                }
                printf("Book alloted succesfully \n\n");
            }
            else
            {
                b->num_request++;
                printf("Sorry Book %s is currently unavailable\n", temp->title_book_asked);
                printf("Sorry recently copy of this book is unavailable your request is registered\n\n");
            }
        }
        else
        {
            printf("Sorry no such book found\n\n");
        }
        prev = temp;
        temp = temp->next;
    }
}


// finding most demand book by number of copies issued + request made that were pending
void mostDemandBook()
{
    book *temp = book_list;
    int max = INT_MIN;
    while (temp != NULL)
    {
        if (temp->num_copies_issued + temp->num_request > max)
        {
            max = temp->num_copies_issued + temp->num_request;
        }
        temp = temp->next;
    }
    temp = book_list; // for printing all max demand books
    while (temp != NULL)
    {
        if (max == temp->num_copies_issued + temp->num_request)
        {
            printf("Book's title is: %s \n", temp->title);
            printf("Book's author is: %s \n", temp->author);
            printf("Book's subject is: %s \n", temp->subject);
            printf("Book's Num of copies issued: %d \n", temp->num_copies_issued);
            printf("Book's Num of copies available: %d \n", temp->num_copies_avail);
            printf("Books Request that were pending are: %d \n\n", temp->num_request);
        }
        temp = temp->next;
    }
}

// merge sort
book *merge(book *l1, book *l2)
{
    book *ptr = (book *)malloc(sizeof(book));
    book *curr = ptr;

     //while either l1 or l2 becomes NULL
    while (l1 != NULL && l2 != NULL)
    {
        if (l1->num_copies_avail <= l2->num_copies_avail)
        {
            curr->next = l1;
            l1 = l1->next;
        }
        else
        {
            curr->next = l2;
            l2 = l2->next;
        }
        curr = curr->next;
    }
    //any remaining Node in l1 or l2 gets inserted in the curr List
    if (l1 != NULL)
    {
        curr->next = l1;
        l1 = l1->next;
    }

    if (l2 != NULL)
    {
        curr->next = l2;
        l2 = l2->next;
    }

    //return the head of the sorted list
    return ptr->next;
}

//function to sort the book_list
book *sort(book *head)
{
    if (head == NULL || head->next == NULL)
    {
        return head;
    }
    book *temp = NULL;
    book *slow = head;
    book *fast = head;
    while (fast != NULL && fast->next != NULL)
    {
        temp = slow;
        slow = slow->next;       // slow increment by 1  // middle of list
        fast = fast->next->next; // fast incremented by 2
    }
    temp->next = NULL;
    book *l1 = sort(head);
    book *l2 = sort(slow);
    book *m = merge(l1 , l2);

    return m;
}


// same as merge book list above 
// merge sort for sorting borrow list according to num of books borrowed by student
borrower *merge_borrow(borrower *l1, borrower *l2)
{
    borrower *ptr = (borrower *)malloc(sizeof(borrower));
    borrower *curr = ptr;
    while (l1 != NULL && l2 != NULL)
    {
        if (l1->num_books >= l2->num_books)
        {
            curr->next = l1;
            l1 = l1->next;
        }
        else
        {
            curr->next = l2;
            l2 = l2->next;
        }
        curr = curr->next;
    }
    if (l1 != NULL)
    {
        curr->next = l1;
        l1 = l1->next;
    }

    if (l2 != NULL)
    {
        curr->next = l2;
        l2 = l2->next;
    }
    return ptr->next;
}

// same as book sort 
borrower *sort_borrow(borrower *head)
{
    if (head == NULL || head->next == NULL)
    {
        return head;
    }
    borrower *temp = NULL;
    borrower *slow = head;
    borrower *fast = head;
    while (fast != NULL && fast->next != NULL)
    {
        temp = slow;
        slow = slow->next;       // slow increment by 1
        fast = fast->next->next; // fast incremented by 2
    }
    temp->next = NULL;
    borrower *l1 = sort_borrow(head);
    borrower *l2 = sort_borrow(slow);
    borrower *m = merge_borrow(l1, l2);

    return m;
}


// books that are not issued by anyone
void books_not_issued_by_anyone()
{
    book *b = book_list;
    while (b != NULL)
    {
        if (b->num_copies_issued == 0)
        {
            printf("Book's title is: %s \n", b->title);
            printf("Book's author is: %s \n", b->author);
            printf("Book's subject is: %s \n", b->subject);
            printf("Book's num of copies available: %d \n", b->num_copies_avail);
            printf("Num of pending reequest were: %d \n\n", b->num_request);
        }
        b = b->next;
    }
}


// checking in requested book in request queue are available or not
void requested_books_available()
{
    student *s = student_list;
    book *b = book_list;
    while (s != NULL)
    {
        b = book_list;

        while (b->next != NULL && strcmp(s->title_book_asked, b->title) != 0)
        {
            b = b->next;
        }
        if (strcmp(s->title_book_asked, b->title) == 0)
        {

            if (b->num_copies_avail > 0)
            {
                printf("Copies available for book %s are %d \n", b->title, b->num_copies_avail);
            }
            else
            {
                printf("No copies are available\n");
            }
        }
        else
        {
            printf("Book not found\n");
        }

        s = s->next;
    }
}


// printing students in request queue who have borrowed book but asking for another book i.e lower priority students
void asking_for_another_book()
{
    student *s = head1_ptr;
    while (s != NULL)
    {
        printf("%s ", s->name);
        printf("%s \n", s->title_book_asked);
        s = s->next;
    }
}

// printing student who were alloted book within 3 days of request date
void get()
{
    borrower *b = borrower_list;
    while (b != NULL)
    {
        Date d = b->req_date; // request date of student for book
        Date alloc_date = b->date;
        addDays(&alloc_date, -15); // allot date

        int i = 0; // counter to count days
        while (d.dd != alloc_date.dd && i <= 3)
        { // date of alloc matched within 3 days then it is a required student node
            addDays(&d, 1);
            i++;
        }
        if (alloc_date.dd == d.dd && alloc_date.mm == d.mm && alloc_date.yyyy == d.yyyy)
        {
            printf("Book was given in %d days to \n", i);
            printf("Student's name: %s \n", b->name);
            printf("Title of book borrowed: %s \n", b->title_book_borrowed);
            printf("Num of book borrowed: %d \n\n", b->num_books);
        }
        b = b->next;
    }
}


// making defaulter list according to current date
// if todays date is greator than current date then shift that borrower node to defaulter list

void make_defaulter_list()
{
    Date d;
    printf("Enter todays date: \n");
    scanf("%d %d %d", &d.dd, &d.mm, &d.yyyy);
    printf("\n");
    borrower *b = borrower_list;
    borrower *b_prev = NULL;
    int count, flag;
    while (b != NULL)
    {
        Date ad = b->date;
        Date bd = b->date;

        addDays(&bd, 1);   // one day after return date so that defaulter list can be made
        addDays(&ad, -15); // allot date
        count = 0;
        flag = 0;
        while ((ad.dd != bd.dd) && !flag)
        { // till allot date reaches return date
            if (ad.dd == d.dd && ad.mm == d.mm && ad.yyyy == d.yyyy)
            { // if current date is in between allot and return date
                flag = 1;
            }
            else
            {
                addDays(&ad, 1);
                count++;
            }
        }
        if (ad.dd == bd.dd && ad.mm == bd.mm && ad.yyyy == bd.yyyy)
        { // if current date is greater than return date shift nodes to defaulter list accordingly
            if (b_prev == NULL)
            {
                borrower_list = borrower_list->next;
                b->next = defaulter_list;
                defaulter_list = b;
                b = borrower_list;
            }
            else
            {
                b_prev->next = b->next;
                b->next = defaulter_list;
                defaulter_list = b;
                b = b_prev->next;
            }
        }
        else if (ad.dd == d.dd && ad.mm == d.mm && ad.yyyy == d.yyyy)
        { // if current date is in between allot and return date
            printf("There are still more %d days to read book %s for %s \n\n", 15 - count, b->title_book_borrowed, b->name);
            b_prev = b;
            b = b->next;
        }
        else
        {
            return;
        }
    }
}


// printing student who borrowed max books according to borrower list
// list is sorted in descending num of borrowed books and accordingly students
// therefore first nodes consist of students with max book borrowed

void max_student_borrowed_book()
{
    if (borrower_list != NULL)
    {
        borrower *b = borrower_list;
        borrower_list = sort_borrow(b); // for sorting acc to num of books borrowed
        borrower *br = borrower_list;

        // first node has borrowed max books as sorting is done
        printf("student name is: %s borrowed max books\n", br->name);
        if (br->next)
        {
            borrower *br_temp = borrower_list->next;

            // checking whether further distinct nodes have max number of borrowed books
            while (br_temp != NULL)
            {
                if (br->num_books == br_temp->num_books && strcmp(br->name, br_temp->name) != 0)
                {
                    printf("student name is: %s borrowed max book\n", br_temp->name);
                }
                br_temp = br_temp->next;
            }
        }
    }
    else
    {
        printf("Borrower list is empty\n");
    }
}


// max request made by student/s in request queue
// checking max request by count
void max_request()
{
    if (student_list != NULL)
    {
        student *s = student_list;
        student *s1 = NULL;
        int count;
        char name[10];
        int max = INT_MIN;
        while (s != NULL)
        {
            s1 = s->next;
            count = 1;
            while (s1 != NULL)
            {
                if (strcmp(s->name, s1->name) == 0)
                {
                    count++; // counting similar named nodes
                }
                s1 = s1->next;
            }
            if (count > max)
            {
                max = count; // updating max
            }
            s = s->next;
        }
        printf("max %d", max);
        s = student_list;
        while (s != NULL)
        {
            s1 = s->next;
            count = 1;
            while (s1 != NULL)
            {
                if (strcmp(s->name, s1->name) == 0)
                {
                    count++;
                }
                s1 = s1->next;
            }
            if (count == max)
            { // printing max nodes
                printf("Max Request i.e %d in request list is by %s \n", max, s->name);
            }
            s = s->next;
        }
    }
    else
    {
        printf("There is no student in request queue\n");
    }
}


// returning book
void return_book()
{
    borrower *b = borrower_list;
    borrower *b_prev = NULL;
    char name[10], title[15];
    printf("Enter name and title of book\n");
    scanf("%s %s", name, title);

    // checking that values matches with either of nodes in borrower list
    while (b != NULL && strcmp(name, b->name) != 0)
    {
        b_prev = b;
        b = b->next;
    }

    // updating book List
    if (b != NULL && strcmp(name, b->name) == 0)
    {
        book *bo = book_list;
        while (bo != NULL && strcmp(title, bo->title) != 0)
        {

            bo = bo->next;
        }

        if (bo != NULL && strcmp(title, bo->title) == 0)
        {
            bo->num_copies_avail++;
            bo->num_copies_issued--;
        }
        else
        {
            printf("please enter author and subject of book to be return");
            char author[15], sub[10];
            scanf("%s %s", sub, author);
            addBooks(b->title_book_borrowed, author, sub, 1);
        }
        if (b_prev == NULL)
        {
            borrower_list = borrower_list->next; // updating borrower list
        }
        else
        {
            b_prev->next = b->next;
        }
        free(b); // freeing that return node
    }
    else
    {
        borrower *d = defaulter_list; // doing same search of return node in default list if node is not found in borrower list
        borrower *d_prev = NULL;
        while (d != NULL && strcmp(name, d->name) != 0)
        {
            d_prev = d;
            d = d->next;
        }

        if (d != NULL && strcmp(name, d->name) == 0)
        {
            book *bk = book_list;
            while (bk != NULL && strcmp(title, bk->title) != 0)
            {
                bk = bk->next;
            }

            if (bk != NULL && strcmp(title, bk->title) == 0)
            {
                bk->num_copies_avail++;
                bk->num_copies_issued--;
            }

            else
            {
                printf("please enter author and subject of book to be return");
                char author[15], sub[10];
                scanf("%s %s", sub, author);
                addBooks(d->title_book_borrowed, author, sub, 1);
            }

            if (d_prev == NULL)
            {
                defaulter_list = defaulter_list->next;
            }

            else
            {
                d_prev->next = d->next;
            }
            free(d);
        }
    }
}


// !! Note : Enter dates same or in increasing order as you enter in real book access system !! //
// note of allot date ,return date, request date enter current / todays date accordingly 

int main()
{
    int choice;
    int done = 0;
    printf("\n!! !! Please Note: Enter dates either same or in increasing order according to management system and instructions correctly !! !!\n");

    while (!done)
    {
        printf("\n*** Enter 1 to add new books to library\n*** Enter 2 to make a request\n*** Enter 3 to find name of students who made max request\n");
        printf("*** Enter 4 to display the names of requested list whose copies are available\n*** Enter 5 to allot books to students\n");
        printf("*** Enter 6 to display students who will get book\n*** Enter 7 to print book most in demand\n*** Enter 8 to sort and display acc to num_of_copies of books available\n");
        printf("*** Enter 9 to display names whose copies are available\n*** Enter 10 to display title of book not issued by anyone\n");
        printf("*** Enter 11 to sort borrower list acc to book issued\n*** Enter 12 to display students already borrowed book and requesting for another\n");
        printf("*** Enter 13 to display students who were given book within 3 days\n*** Enter 14 to display students who borrowed maximum number of books\n");
        printf("*** Enter 15 to return the borrowed book\n*** Enter 16 to print book list\n*** Enter 17 to print borrower list\n");
        printf("*** Enter 18 to make and print defaulter list\n*** Enter 19 to print request queue\n*** Enter 20 to print defaulter list\n*** Enter 21 to exit from program\n");
        scanf("%d", &choice);
        printf("\n");
        book *b = book_list;
        borrower *br = borrower_list;
        switch (choice)
        {
        case 1:
            addBooks();
            break;
        case 2:
            request_queue();
            break;
        case 3:
            max_request();
            break;
        case 4:
            requested_books_available();
            break;
        case 5:
            alloc_book();
            break;
        case 6:
            alloc_book();
            display_borrower_list();
            break;
        case 7:
            alloc_book();   // first allocate books to student and then find max demand in borrower list 
            mostDemandBook();
            break;
        case 8:
            book_list = sort(b);
            break;
        case 9:
            requested_books_available();
            break;
        case 10:
            books_not_issued_by_anyone();
            break;
        case 11:
            borrower_list = sort_borrow(br);
            break;
        case 12:
            asking_for_another_book();
            break;
        case 13:
            get();
            break;
        case 14:
            max_student_borrowed_book();
            break;
        case 15:
            return_book();
            break;
        case 16:
            display_book_list();
            break;
        case 17:
            display_borrower_list();
            break;
        case 18:
            make_defaulter_list();
            printf("\n");
            display_defaulter_list();
            break;
        case 19:
            display_list();
            break;
        case 20:
            display_defaulter_list();
            break;
        case 21:
            done = 1;
            break;
        default:
            done = 1;
            break;
        }
    }
}
