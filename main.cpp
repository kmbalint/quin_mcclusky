#include "graphics.hpp"
#include <iostream>
#include <stdio.h>
#include <sstream>
#include <list>
#include <vector>


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





};


int main()
{

    gout.open(800,800);

    event ev;

    digitbox input(200,200);





    gin.timer(20);

    while(gin >> ev) {

        if(ev.type == ev_timer) {
            gout << move_to(0,0) << color(0,0,0) << box(800,800);

            input.draw();

        }

        if(ev.type == ev_mouse) {

            input.setFocus(ev.pos_x,ev.pos_y);
        }

        if(ev.button == btn_left) {

            input.activate();
        }


        gout << refresh;
    }
    return 0;
}
