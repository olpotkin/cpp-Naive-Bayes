#include "classifier.h"
#include <iostream>
#include <fstream>
#include <math.h>
#include <vector>

using namespace std;

vector<vector<double>> Load_State(string file_name) {
  ifstream in_state_(file_name.c_str(), ifstream::in);
  vector<vector<double>> state_out;
  string line;

  while (getline(in_state_, line)) {
    istringstream iss(line);
    vector<double> x_coord;
    string token;
    while(getline(iss, token, ',')) {
      x_coord.push_back(stod(token));
    }
    state_out.push_back(x_coord);
  }

  return state_out;
}


vector<string> Load_Label(string file_name) {
  ifstream in_label_(file_name.c_str(), ifstream::in);
  vector<string> label_out;
  string line;

  while (getline(in_label_, line)) {
    istringstream iss(line);
    string label;
    iss >> label;

    label_out.push_back(label);
  }

  return label_out;
}


int main() {
  // Each coordinate contains 4 pieces of information:
  // s - distance along the road (longitudinal displacement)
  // d - side-to-side position on the road (lateral displacement)
  // s_dot - velocity of the car along the s axis (for longitudinal displacement)
  // d_dot - velocity of the car along the d axis (for lateral displacement)
  vector<vector<double>> X_train_f = Load_State("./pred_data/train_states.txt");
  vector<vector<double>> X_test_f  = Load_State("./pred_data/test_states.txt");

  // Remove feature [0] 's' from training set
  int train_dim_1 = X_train_f.size();
  int train_dim_2 = X_train_f[0].size();
  vector<vector<double>> X_train(train_dim_1, vector<double>(train_dim_2 - 1));
  for (size_t i = 0; i < train_dim_1; ++i) {
    for (size_t j = 1; j < train_dim_2; ++j) {
      X_train[i][j-1] = X_train_f[i][j];
    }
  }
  // Remove feature [0] 's' from test set
  int test_dim_1 = X_test_f.size();
  int test_dim_2 = X_test_f[0].size();
  vector<vector<double>> X_test(test_dim_1, vector<double>(test_dim_2 - 1));
  for (size_t i = 0; i < test_dim_1; ++i) {
    for (size_t j = 1; j < test_dim_2; ++j) {
      X_test[i][j-1] = X_test_f[i][j];
    }
  }

  // Action:
  // - left
  // - right
  // - keep
  vector<string> Y_train  = Load_Label("./pred_data/train_labels.txt");
  vector<string> Y_test   = Load_Label("./pred_data/test_labels.txt");

  cout << "X_train number of elements " << X_train.size() << endl;
  cout << "X_train element size " << X_train[0].size() << endl;
  cout << "Y_train number of elements " << Y_train.size() << endl;

  GNB gnb = GNB();

  gnb.train(X_train, Y_train);

  cout << "X_test number of elements " << X_test.size() << endl;
  cout << "X_test element size " << X_test[0].size() << endl;
  cout << "Y_test number of elements " << Y_test.size() << endl;

  int score = 0;
  for(int i = 0; i < X_test.size(); i++) {
    vector<double> coords = X_test[i];
    string predicted = gnb.predict(coords);
    if(predicted.compare(Y_test[i]) == 0) {
      score += 1;
    }
  }

  float fraction_correct = float(score) / Y_test.size();
  cout << "You got " << (100 * fraction_correct) << " correct" << endl;

  return 0;
}
