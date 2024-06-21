#include <iostream>
#include <string>

using namespace std;

int main1(int argc, char *argv[]) {
/*argv[]"는 명령행 인자를 저장하는 문자열(char) 배열의 포인터를 나타냅니다.
즉, argv[]는 프로그램이 실행될 때 명령 프롬프트에서 전달된 인자들의 문자열을 포인터 배열 형태로 저장하고 있습니다. 
이 배열에는 프로그램 이름을 가리키는 첫 번째 인자와 그 외의 인자들이 저장됩니다.*/
  string s1 = "Hello, C++!";
  cout << "s1=" << s1 << endl;
  string s0 = "Hello";  //문자열 class String객체의 좌변은 객체, 우변은 문자열이지만, 오버로딩을 지원해서 우변은 객체가 아니어도 됨
  string s2 = s0 + ", " + "C++" + '!';
  cout << "s2=" << s2 << endl;

  int len1 = s1.length();
  int len2 = s2.length();
  cout << "len1=" << len1 << endl;
  cout << "len2=" << len2 << endl;

  int b1 = s1.compare(s1); // compare s1 and s2 (in content) if compare의 return값이 0이면 동일
  int b2 = s1.compare(s2); // compare s1 and s2 (in content)
  bool b3 = (s1 == s2); // compare s1 and s2 (in content) bool자료형은 1이면 True

  cout << "s1.compare(s1)=" << b1 << endl;
  cout << "s1.compare(s2)=" << b2 << endl;
  cout << "(s1==s2)=" << b3 << endl;
  return 0;
}

//------------------------------------------------------------------------//
// Patch to Cygwin's bug: stoi, stod, to_string are not supported!
#include <cstdlib> // for strtol, strtod
#include <sstream> // for to_string
namespace patch {
  template < typename T > std::string to_string( const T& n )  {
    std::ostringstream stm ;
    stm << n ;
    return stm.str() ;
  }
}
using namespace patch;
//------------------------------------------------------------------------//

int main2(int argc, char *argv[]) {
  string istr = "1234";
  string dstr = "12.34";

  //int ival = stoi(istr); // string -> integer value
  //double dval = stod(dstr); // string -> double value
  int ival = strtol(istr.c_str(), NULL, 10); // string -> integer value strtol:long int
  double dval = strtod(dstr.c_str(), NULL); // string -> double value
  //(변수or객체.c_str():.앞의 주소를 달라,계산할 수 없는 문자를 발견하면 NULL,10진수)
  cout << "before : " << ival << ", " << dval << endl;

  ival = ival + 1111;
  dval = dval + 11.11;
  string istr2 = to_string(ival); // integer value -> string
  string dstr2 = to_string(dval); // double value -> string
  //string함수 내 소수점 포맷은 6자리

  cout << "after  : " << istr2 << ", " << dstr2 << endl;
  return 0;
}

void printArray(int a[], int len) {
  if (a != NULL) {
    for (int i = 0; i < len; i++)
      cout << a[i] << " ";
  }
  cout << endl;
}

#include <vector>
#include <iterator>
int main3(int argc, char *argv[]) {
  int* A0 = NULL;  //int A0[] = NULL;
  int A1[] = {1, 2, 3, 4, 5};
  int* A2 = new int[5]; //int[] A2 = new int[5];
  int* A3 = new int[5]{1, 2, 3, 4, 5}; //int A3[] = new int[]{1, 2, 3, 4, 5};
/*배열의 이름은 배열의 시작주소이다.(배열은 포인터 변수: 시작 주소값을 갖고 있는 const변수)
포인터 변수는 객체 타입이 아님
cout << "A0:" << A0;로 표현 못하는 이유: 포인터 변수가 배열 요소의 개수 정보를 갖고 있지 않음*/
  cout << "A0:";  printArray(A0, 0);
  cout << "A1:";  printArray(A1, 5);
  cout << "A2:";  printArray(A2, 5);
  cout << "A3:";  printArray(A3, 5);

  vector<int> v1(A1, A1 + 5); //vector는 가변길이 배열
  vector<int> v3(A3, A3 + 5); //포인터가 가리키는 마지막 주소는 마지막 원소의 다음 주소임
  cout << "equal(v1, v3)=" << equal(v1.begin(), v1.end(), v3.begin());
  //v1의 시작에서 끝만큼 v3를 비교
  cout << endl;
  //v1.push_back(9)은 v1[5] = 9; 이렇게 배열의 길이를 늘리려면 push_back사용

  return 0;
}

class Nested {
public: // private:
  int dy; // dynamic variable static이 아니니까 개별 변수
  static int dx; // static variable 클래스 안에서 공유하는 변수
public:
  Nested(int cy, int cx) { dy = cy; dx = cx; }
  int get_dy() { return dy; } //static이 아니니까 개별 객체 함수
  static int get_dx() { return dx; } // can be declared 'dynamic'
  //static함수는 클래스 대표 객체에 저장되며 다이나믹 변수에 접근 못함. why? static함수가 다이나믹 변수에 접근할 정보가 없음
  // class InnerD {
  // public:
  //   int get_dy() { return dy; }
  //   int sum() { return dy+dx; }
  //   int get_dx() { return dx; }
  // };
  class InnerS {  // An inner class inherently is a static class
                  // even without 'static' keyword
                  //outer class의 static함수 or 변수만 사용 가능, 다이나믹은 사용 불가
  public:
    int get_dx() { return dx; } 
  };
};
//int add() { static int A;}이 static은 할당해지 되지 않는 영원히 남아있는 변수, 공유 변수 개념이 아님
int Nested::dx = 0;

int main4(int argc, char *argv[]) {
  Nested* m1 = new Nested(1, 2);
  Nested* m2 = new Nested(3, 4);
  cout << "m1->get_dy()=" << m1->get_dy() << ", m1->get_dx()=" << m1->get_dx() << endl;
  cout << "m2->get_dy()=" << m2->get_dy() << ", m2->get_dx()=" << m2->get_dx() << endl;
  //Nested::InnerD *d1 = m1::InnerD();
  //Nested::InnerD *d2 = m2::InnerD();
  Nested::InnerS *s = new Nested::InnerS();
  cout << "s->get_dx()=" << s->get_dx() << endl;

  return 0;
}

#include "Matrix.h"
class MyMatrix : public Matrix {//public함수로 작성하면 부모의 private변수도 접근 가능.
public://부모 를래스와 동일한 형식으로 생성자 작성
  MyMatrix() : Matrix() {}
  MyMatrix(int cy, int cx) : Matrix(cy, cx) { }
  MyMatrix(int *arr, int col, int row) : Matrix(arr, col, row) { }
  void print() {
    int dy = get_dy(); 
    int dx = get_dx();
    int **array = get_array();
    for (int y=0; y < dy; y++) {
      for (int x=0; x < dx; x++) {
        if (array[y][x] == 0) cout << "□ ";
        else if (array[y][x] == 1) cout << "■ ";
	else cout << "X ";
      }
      cout << endl;
    }
  }
};

int main5(int argc, char *argv[]) {
  Matrix* m1 = new Matrix(3,3);
  m1->print(); cout << endl;
  int A[] = { 0, 1, 0, 1, 1, 1, 0, 0, 0 }; // int A[][]
  Matrix* m2 = new Matrix(A, 3, 3); // new Matrix(A) //m2는 Matrix 타입을 가리키는 포인터 변수, 포인터 변수의 타입이 Matrix포인터 변수
  m2->print(); cout << endl;
  MyMatrix* m3 = new MyMatrix(3,3);
  m3->print(); cout << endl;
  MyMatrix* m4 = new MyMatrix(A, 3, 3); // new Matrix(A)
  m4->print(); cout << endl;
  m2 = m4; // polymorphism: Matrix covers MyMatrix!! m2의 주소가 MyMatrix 객체의 주소로 교환.
  /*m2는 MyMatrix클래스의 객체를 가리키도록 변경되지만 
  여전히 m2의 타입은 Matrix포인터 변수이기 때문에 m2가 가리키는 객체의 타입과 관계없이 메소드는 정적바인딩에 의해 결정.
  만약 m2가 MyMatrix타입의 포인터 변수였으면 동적 바인딩에 의해 결정.(MyMatrix클래스의 print()이용).
  상속받은 m4를 부모 class에 대입해도 정상적으로 동작*/
  m2->print(); cout << endl; // static binding!! //정적 바인딩의 예시로 부모의 print호출

  return 0;
}

#include <string.h>
int main(int argc, char *argv[]) {
  if (argc != 2) {
    cout << "usage: " << argv[0] << " [function number]" << endl;
    return 0;
  }
    
  if (strcmp(argv[1], "1") == 0) main1(argc, argv);
  else if (strcmp(argv[1], "2") == 0) main2(argc, argv);
  else if (strcmp(argv[1], "3") == 0) main3(argc, argv);
  else if (strcmp(argv[1], "4") == 0) main4(argc, argv);
  else if (strcmp(argv[1], "5") == 0) main5(argc, argv);
  else
    cout << "usage: " << argv[0] << " [function number]" << endl;
  return 0;
}


  
