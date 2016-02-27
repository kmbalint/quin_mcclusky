#include "graphics.hpp"
#include <iostream>
#include <stdio.h>
#include <sstream>
#include <list>
#include <vector>
#include <cmath>


using namespace genv;

std::string convertToString(int i) {
    std::stringstream ss;
    std::string s;

    ss << i;
    ss >> s;

    ss.clear();

    return s;
}

std::list<int> convert_to_binary(int dec){

    std::list<int> bin;

    int b = dec % 2;

    bin.push_front(b);

    while(dec > 1){

        dec = (dec-b)/2;

        b = dec % 2;

        bin.push_front(b);

    }

    for (std::list<int>::iterator it = bin.begin(); it != bin.end(); it++){
    std::cout << *it << ' ';};

    return bin;

    }

int bw_count(int dec){

    int bin_weight = 0;

    int b = dec % 2;

    while(dec > 0){

        bin_weight = bin_weight + b;

        dec = (dec-b)/2;

        b = dec % 2;
    }

    std::cout<<bin_weight;
    return bin_weight;

    }


struct minterm{

    int mt;

    std::list<int> bin = convert_to_binary(mt); //ne felejtsd hogy itt forditva kell majd kiolvasni a helyi�rt�kek miatt!

    int bin_weight = bw_count(mt);

};

bool is_neighbour(std::vector<minterm> m1, std::vector<minterm> m2){

    bool neighbour = false;

    int i = 0;

    int mt1_sum = 0;

    int mt2_sum = 0;

    int bin_diff = 0;

    bool is_bin_diff_one = true;

    bool is_higher_than = true;

    while(i < m1.size()){

        mt1_sum = mt1_sum + m1[i].mt;

        mt2_sum = mt2_sum + m2[i].mt;

        bin_diff = bin_diff + std::abs(m1[i].bin_weight - m2[i].bin_weight);

        if (std::abs(m1[i].bin_weight - m2[i].bin_weight) != 1) is_bin_diff_one = false;

        int j = 0;

        while(j < m2.size()){

            if ((m1[i].mt > m2[j].mt && m1[i].bin_weight > m2[i].bin_weight) || (m1[i].mt < m2[j].mt && m1[i].bin_weight < m2[i].bin_weight)) is_higher_than = false;

        }

        i++;

    }

    int mt_sum = mt1_sum + mt2_sum;

    bool is_power_of_two = true;

    while (((mt_sum % 2) == 0) && mt_sum > 1) {

        mt_sum /= 2;

    }

    if (mt_sum != 1) is_power_of_two = false;

    if (is_bin_diff_one && is_higher_than && is_power_of_two) neighbour = true;

    return neighbour;

}


std::vector<minterm> make_new_implicant(std::vector<minterm> m1, std::vector<minterm> m2){

    // std::vector<minterm> m12;

    if (is_neighbour(m1,m2)) m1.insert(m1.end(), m2.begin(), m2.end());

    return m1;

}


struct digitbox{



    int peek_x, peek_y, w, h, value;

    bool focus_up;
    bool focus_down;

    digitbox(int _peek_x, int _peek_y):peek_x(_peek_x),peek_y(_peek_y),w(75),h(25),value(0),focus_up(false),focus_down(false){};

    void draw(){

        gout << move_to(peek_x,peek_y) << color(200,200,200) << box(w,h) << move(-w+2,-h+2) << color(150,150,150) << box(w-2,h-2);

        if(focus_down) gout << color(255,165,0) << move_to(peek_x+2,peek_y+2) << box((w-4)/3,h-4);
        if(focus_up) gout << color(255,165,0) << move_to(peek_x+w-2,peek_y+2) << box(-(w-4)/3,h-4);

        gout << color(0,0,0) << move_to(peek_x+10,peek_y+h/2+5) << text("<");
        gout << color(0,0,0) << move_to(peek_x+w-20,peek_y+h/2+5) << text(">");

        gout << color(0,0,0) << move_to(peek_x+w/2-3,peek_y+h/2+5) << text(convertToString(value));
    }

    void setFocus(int ex, int ey) {

        if( peek_x < ex && peek_x+w/3 > ex && peek_y < ey && peek_y+h > ey ) focus_down=true;
        else focus_down = false;
        if( peek_x+2*w/3 < ex && peek_x+w > ex && peek_y < ey && peek_y+h > ey) focus_up=true;
        else focus_up = false;

    }

    void activate() {

        if( focus_down ) value--;
        if( focus_up ) value++;

        if( value < 0 ) value=0;
        if( value > 9 ) value=9;
    }

    int getValue() {return value;}

};

struct block {

    int peek_x, peek_y, value, w, h;

    block ( int _peek_x, int _peek_y, int _value) : peek_x(_peek_x),peek_y(_peek_y),value(_value),w(25),h(25) {}

    int getX() {return peek_x;}
    int getY() {return peek_y;}

    void draw() {
        gout << move_to(peek_x+1,peek_y+1) << color(150,150,150) << box(w-2,h-2) << move_to(peek_x,peek_y+22) << color(0,0,0) << text(convertToString(value));
    }

};

std::vector<block *> mirror(std::vector<block *> b, bool right, int x, int y, int w, int h) {

    std::vector<block *> ret = b;

    for(size_t i = 0; i < b.size(); i++) {
        if(right) ret.push_back(new block(2*x+w-b[i]->getX()-25,b[i]->getY(),i+b.size()));
        else ret.push_back(new block(b[i]->getX(),2*y+h-b[i]->getY()-25,i+b.size()));
    }

    return ret;

}

std::vector<block *> split(std::vector<block *> b) {

    std::vector<block *> ret;

    for(size_t i = 0; i < b.size(); i++) {
        if( i < b.size()/2 ) ret.push_back(b[i]);
        else delete b[i];
    }

    return ret;
}

struct table {

    int peek_x, peek_y, input, w, h;

    std::vector<block *> blocks;

    table ( int _peek_x, int _peek_y) : peek_x(_peek_x),peek_y(_peek_y),input(0){
        update(input);
    }

    void draw() {
        if(input!=0) {
            gout << move_to(peek_x-1,peek_y-1) << color(200,200,200) << box(w+2,h+2);
            for(size_t i = 0; i < blocks.size(); i++) blocks[i]->draw();
        }
    }

    void update( int in ) {

        if(in == 0) {
            w=0;
            h=0;
        }
        else {
            w=pow(2,(in+1)/2)*25;
            h=pow(2,in/2)*25;
        }

        while(input > in) {
            input--;
            if(input == 0) {
                blocks.clear();
            }
            else blocks = split(blocks);
        }

        while(input < in) {
            input++;
            if(input == 1) {
                blocks.push_back(new block(peek_x,peek_y,0));
                blocks.push_back(new block(peek_x+25,peek_y,1));
            }
            else if(input%2==0)blocks = mirror(blocks,false,peek_x,peek_y,w,h);
            else if(input%2==1)blocks = mirror(blocks,true,peek_x,peek_y,w,h);
        }

        std::cout << blocks.size() << std::endl;
    }
};


int main()
{

    gout.open(1600,900);

    event ev;

    digitbox input(100,100);

    table T(100,200);


    std::vector<std::vector<int> > prime_implicants;

    gin.timer(20);

    while(gin >> ev) {

        if(ev.type == ev_timer) {
            gout << move_to(0,0) << color(0,0,0) << box(1600,900);

            input.draw();

            T.draw();

        }

        if(ev.type == ev_mouse) {

            input.setFocus(ev.pos_x,ev.pos_y);
        }

        if(ev.button == btn_left) {

            input.activate();

            T.update(input.getValue());

        }


        gout << refresh;
    }
    return 0;
}
