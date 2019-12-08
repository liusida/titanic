/*
 * Exp 3 is an attempt to do an evolutionary experiment. Although failed, it is a runnable experiment, and I wrote several tools that will be useful in the future.
 */

#include <iostream>
#include <vector>
#include <fstream>

#include <cmy_sim.h>

using namespace std;
#include <nlohmann/json.hpp>
using json = nlohmann::json;

#define random_double_number ((double) rand()/RAND_MAX)
struct Node {
    Vec position;
    double damping=0.9996;
};
struct Edge {
    unsigned left_node;
    unsigned right_node;
    unsigned omega=0; //frequency
    unsigned type=PASSIVE_STIFF;
};

class RobotX : public Container {
public:
    void saveJson() {
        json j;
        for (auto n:nodes) {
            json j1;
            j1["damping"] = n.damping;
            j1["x"] = n.position[0];
            j1["y"] = n.position[1];
            j1["z"] = n.position[2];
            j["nodes"].push_back(j1);
        }
        for (auto e:edges) {
            json j1;
            j1["left"] = e.left_node;
            j1["right"] = e.right_node;
            j1["omega"] = e.omega;
            j1["type"] = e.type;
            j["edges"].push_back(j1);
        }
        std::ofstream file("models/exp_3.json");
        file << j.dump(4);
    }
    void loadJson() {
        std::ifstream file("models/exp_3.json");
        json j = json::parse(file);
        for (auto n:j["nodes"]) {
            nodes.push_back( { Vec(n["x"], n["y"], n["z"]), n["damping"]});
        }
        for (auto e:j["edges"]) {
            edges.push_back({e["left"], e["right"], e["omega"], e["type"]});
        }
    }
    void generateFromRandom(unsigned number_of_nodes=10, double connection_probability=0.7) {
        //Random position nodes
        for (unsigned i=0;i<number_of_nodes;i++) {
            nodes.push_back( { Vec(random_double_number, random_double_number, random_double_number) });
        }
        //probability ER random edges
        for (unsigned i=0;i<nodes.size();i++) {
            for (unsigned j=i+1;j<nodes.size();j++) {
                if (random_double_number<connection_probability) {
                    //left_node, right_node, omega frequency, type
                    edges.push_back({i,j, (unsigned)rand()%4+1, (unsigned)rand()%4});
                }
            }
        }
    }
    void putIntoSimulation(CMySimulation& sim) {
        // build a robot from nodes and edges
        masses.clear();
        for (auto n:nodes) {
            Mass *m = sim.createMass(n.position);
            m->damping = n.damping;
            masses.push_back(m);
        }
        springs.clear();
        for (auto e:edges) {
            Spring * s = sim.createSpring(masses[e.left_node], masses[e.right_node]);
            s->_omega = e.omega;
            s->_type = e.type;
            springs.push_back(s);
        }
    }
    RobotX * reproduce() {
        RobotX * child = new RobotX(*this);
        for (auto &e:child->edges) {
            if (random_double_number<0.1) {
                e.omega = rand()%4;
            }
        }
        child->masses.clear();
        child->springs.clear();
        return child;
    }
    double measure() {
        if (horizontal_distance_from_origin>0)
            return horizontal_distance_from_origin;
        else {
            Vec total_position(0,0,0);
            for (auto m:masses) {
                total_position += m->pos;
            }
            total_position = total_position / masses.size();
            total_position[2] = 0; //ignore z-index
            center_of_mass = total_position;
            horizontal_distance_from_origin = total_position.norm();
            return horizontal_distance_from_origin;
        }
    }
    void resetMeasure() {
        horizontal_distance_from_origin = 0;
    }
    vector<Node> nodes;
    vector<Edge> edges;
    Vec center_of_mass = {0,0,0};
    double horizontal_distance_from_origin=0;
};


int main() {
    CMySimulation *sim = new CMySimulation();
    sim->setSkyColor({0.6, 0.6, 0.7, 0.0});
    sim->createPlane(Vec(0, 0, 1), -1); // create constraint plane
    CMyContactPlane *p = (CMyContactPlane *)sim->lastConstraint();
    p->setPlaneColor({.3,.3,.3}, {.2,.2,.3});

    
    unsigned population = 20;

    vector<RobotX *> robots;
    RobotX * Adam = new RobotX();
    //Adam->generateFromRandom();
    Adam->loadJson();
    for (unsigned i=0;i<population;i++) {
        RobotX* r = Adam->reproduce();
        r->putIntoSimulation(*sim);
        robots.push_back(r);
    }

    sim->start();

    for (unsigned i=0;i<999;i++) {
  
        std::this_thread::sleep_for(std::chrono::milliseconds(10000));
        sim->pause(0.);
        sim->getAll();

        vector<double> distances;
        for (auto r:robots) {
            r->resetMeasure();
            distances.push_back(r->measure());
        }
        sort(distances.begin(), distances.end());
        for (auto d:distances)
            cout<<d<<endl;
        double threshold = distances[(unsigned)distances.size()/10];
        cout<<threshold<<endl;
        unsigned n_killed = 0;
        for (unsigned i=0; i<robots.size(); i++ ) {

            RobotX * r = robots[i];
            if (r->measure()<threshold) {
                sim->deleteContainer(r);
                robots.erase( robots.begin() + i );
                n_killed ++;
            }
            if (r->measure()==distances[distances.size()-1]) {
                r->saveJson();
            }
        }
        if (n_killed==population) {
            cout<<"Die Out. Game Over."<<endl;
            break;
        }

        for (unsigned i=robots.size();i<population;i++) {
            RobotX* r = robots[rand()%robots.size()]->reproduce();
            r->putIntoSimulation(*sim);
            robots.push_back(r);
        }


        if ((i-1)%10==0) { //reset all position
            for (auto r:robots) {
                for (auto m:r->masses) {
                    m->pos -= r->center_of_mass;
                }
            }
        }
        sim->setAll();
        sim->resume();

    }
}