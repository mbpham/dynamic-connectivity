#include <iostream>
#include "lib.cpp"
using namespace std;

int main(int argc, char** argv)
  {
    graph_t* a;
    a = makeGraph(500);
    addEdge(a,1,2);
    addEdge(a,4,3);
    addEdge(a,5,3);
    addEdge(a,1,4);
    //addEdge(a,6,31);
    //addEdge(a,6,30);
    //addEdge(a,6,33);
    //addEdge(a,5,10);
    //addEdge(a,5,30);
    //addEdge(a,5,4);
    //deleteEdge(a,4,3;)
    //deleteEdge(a,2,3);
    //prettyPrinting(a);

    int i;

  }
/*
  struct employee{
      char gender[13];
      char name[13];
      int id;
    };

    int compare(const void *s1, const void *s2)
    {
      printf("ss\n");
      struct employee *e1 = (struct employee *)s1;
      struct employee *e2 = (struct employee *)s2;
      int gendercompare = strcmp(e1->gender, e2->gender);
      if (gendercompare == 0)
        return e1->id - e2->id;
      else
        return -gendercompare;
    }

    int main()
    {
      int i;
      struct employee info[]={{"male","Matt",1234},{"female","Jessica",2345},{"male","Josh",1235}};

      for (i = 0; i < 3; ++i)
        printf("%d\t%s\t%s\n", info[i].id, info[i].gender, info[i].name);

      qsort(info, 3, sizeof(struct employee), compare);

      for (i = 0; i < 3; ++i)
        printf("%d\t%s\t%s\n", info[i].id, info[i].gender, info[i].name);
    }
*/
