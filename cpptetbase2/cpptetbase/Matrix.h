#pragma once
#include <iostream>
#include <cstdlib>

using namespace std;

class Matrix {
private:
  static int nAlloc;
  static int nFree;
  int dy;
  int dx;
  int **array;
  void alloc(int cy, int cx);
  void dealloc();
public:
  static int get_nAlloc(); 
  //static함수는 멤버 변수를 사용할 수 없음 해당 클래스의 static변수나 정적 함수 내부에 선언된 변수만 가능
  //멤버 함수는 멤버 변수, 정적 변수 모두 가능
  //this 포인터를 사용할 수 없음
  //static으로 선언하면 클래스의 인스턴스 없이도 호출할 수 있음 ex: Matrix::get_nAlloc()
  static int get_nFree();
  int get_dy() const;
  int get_dx() const;
  int** get_array() const;
  Matrix();
  Matrix(int cy, int cx);
  Matrix(const Matrix *obj);
  Matrix(const Matrix &obj);
  Matrix(int *arr, int col, int row);
  ~Matrix();
  Matrix *clip(int top, int left, int bottom, int right);
  Matrix clip_(int top, int left, int bottom, int right);
  void paste(const Matrix *obj, int top, int left);
  void paste(const Matrix &obj, int top, int left);
  Matrix *add(const Matrix *obj);
  friend const Matrix operator+(const Matrix& m1, const Matrix& m2);
  //const Matrix operator+(const Matrix& m2) const;
  int sum();
  void mulc(int coef);
  Matrix *int2bool();
  bool anyGreaterThan(int val);
  void print();
  friend ostream& operator<<(ostream& out, const Matrix& obj);
  Matrix& operator=(const Matrix& obj);
};
