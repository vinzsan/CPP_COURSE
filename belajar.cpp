#include <iostream>

void change_val(int &val,int new_value){
  val = new_value;
}

void change_val_by_dereferencing(int val,int new_val){
  *(int *)&val = new_val; // atau bisa juga dengan *val = new_val;
}

int main(){

}
