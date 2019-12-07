#include <iostream>
#include <vector>

#include <sim.h>

using namespace std;

#define parameter_space 2
#define population_size 20
void cppn_generator(cppn& encoding)
{
    for(unsigned i = 0; i < parameter_space; i++)
    {
      vector<vector<vector<int>>> w;
      encoding.push_back( w );
      for(unsigned j = 0; j < parameter_space; j++)
      {
        vector<vector<int>> v;
        encoding[i].push_back( v );
        for(unsigned k = 0; k < parameter_space; k++)
        {
          vector<int> u;
          encoding[i][j].push_back( u );
          for (unsigned l = 0; l< parameter_space; l++)
          {
            encoding[i][j][k].push_back(rand()%2);
          }
        }
      }
    }
}
void cppn_modifier(cppn& encoding) {
    for (unsigned i=0;i<parameter_space;i++) {
        for (unsigned j=0;j<parameter_space;j++) {
            for (unsigned k=0;k<parameter_space;k++) {
                for (unsigned l=0;l<parameter_space;l++) {
                    if (rand()%10>8) {
                        encoding[i][j][k][l] = rand()%2;
                    }
                }
            }
        }
    }
}



int main() {
  vector <Robot *> robots;
  vector<cppn> encodings;

  Simulation sim;
  sim.createPlane(Vec(0, 0, 1), 0); // create constraint plane x-y, z=0

  for(int i=0;i<population_size;i++) {
      cppn t;
      cppn_generator(t);
      Robot * r = sim.createRobot(Vec(2*i-10,-2*i+10,1), t, 1.0, 5);

      encodings.push_back(t);
      robots.push_back(r);
  }
  cout<<"Number of robots: "<<robots.size()<<endl;
  sim.start();
  for (unsigned round=0;round<500;round++) {
      cout<<"start round "<<round<<endl;
      std::this_thread::sleep_for(std::chrono::seconds(10)); //after they are stable...
      cout<<"stable"<<endl;
      sim.getAll();
      vector<Vec> position_after_stable;
      for (unsigned i=0;i<robots.size();i++) {
          if (robots[i]->masses.size()>0)
            position_after_stable.push_back(robots[i]->masses[0]->pos);
          else
            position_after_stable.push_back(Vec(0,0,0));
      }
      std::this_thread::sleep_for(std::chrono::seconds(30)); //let them walk...
      cout<<"pause"<<endl;
      sim.pause(0.0);
      sim.getAll();
      vector<double> distances_of_walk;
      for (unsigned i=0;i<robots.size();i++) {
          Vec p;
          if (robots[i]->masses.size()>0)
              p = robots[i]->masses[0]->pos;
          else
              p = Vec(0,0,0);
          double d = (p - position_after_stable[i]).norm();
          distances_of_walk.push_back(d);
      }
      vector<unsigned> sort_indices(distances_of_walk.size());
      size_t n(0);
      generate(sort_indices.begin(), sort_indices.end(), [&]{return n++;});
      sort(sort_indices.begin(), sort_indices.end(), [&](unsigned d1, unsigned d2){return distances_of_walk[d1]>distances_of_walk[d2];});
      vector<cppn> new_encodings;
      for (unsigned i=0;i<sort_indices.size()/2;i++) {
          // save faster half and copy one more
          new_encodings.push_back(encodings[sort_indices[i]]);
          cppn child = encodings[sort_indices[i]];
          cppn_modifier(child);
          new_encodings.push_back(child);
      }
      encodings = new_encodings;
      for(unsigned i=0;i<robots.size();i++) {
          sim.deleteContainer(robots[i]);
      }
      for(int i=0;i<population_size;i++) {
          robots[i] = sim.createRobot(Vec(2*i-10,-2*i+10,1), encodings[i], 1.0, 5);
      }
      sim.resume();
  }
  sim.stop();
  return 0;

}
