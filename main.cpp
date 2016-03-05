#include "graphics.hpp"
#include <iostream>
#include <stdio.h>
#include <sstream>
#include <list>
#include <vector>
#include <cmath>


using namespace genv;

const int X = 1600;
const int Y = 900;

std::list<int> convertIntToBinary(int dec) {

    std::list<int> bin;

    int b = dec % 2;

    bin.push_front(b);

    while(dec > 1){

        dec = (dec-b)/2;

        b = dec % 2;

        bin.push_front(b);

    }

    return bin;
}

std::string convertIntToString(int i) {
    std::stringstream ss;
    std::string s;

    ss << i;
    ss >> s;

    ss.clear();

    return s;
}

int convertStringToInt(std::string str) {
    std::stringstream ss;
    int i;

    ss << str;
    ss >> i;

    ss.clear();

    return i;
}

int getBinaryWeight(std::list<int> bin) {
    int bin_weight = 0;

    for (std::list<int>::iterator it = bin.begin(); it != bin.end(); it++) bin_weight += *it;

    return bin_weight;
}

int getCharPos(char c, std::string s) {
    for(size_t i = 0; i < s.size(); i++) {
        if(s[i] == c) return i;
    }
    return -1;
}

class base {

protected:

    int x,y,weight,height;

    bool focus, active;

    std::string value;

public:

    base(int _x, int _y, int _weight, int _height, std::string _value) : x(_x), y(_y), weight(_weight), height(_height), focus(false), active(false), value(_value) {

        //if(weight < 25) weight = 25;
        //if(height < 25) height = 25;

    }

    std::string getValue() {

        return value;

    }

    virtual void draw() {

        if(x >= 0 && x < X && y >= 0 && y < Y){
            setColor();
            gout << move_to(x,y) << box(weight,height) << color(0,0,0) << move_to(x+1,y+1) << box(weight-2,height-2);
            setColor();
            gout << move_to(x+5,y+height/2+5) << text(value);
        }

    }

    virtual void setFocus(int _ex, int _ey) {

        if(_ex >= x && _ey >= y && _ex < x+weight && _ey < y+height ) focus = true;
        else focus = false;

    }

    virtual void setColor() {

        if(focus) gout << color(155,65,0);
        else if(active) gout << color(255,165,0);
        else gout << color(100,100,100);

    }

    virtual void setEvent(event e) {

        if(e.type == ev_timer) draw();

        if(e.type == ev_mouse) setFocus(e.pos_x,e.pos_y);

        if(e.button == btn_left) setActive();

    };

    virtual void setActive() = 0;
};

class textbox : public base {

public:

    textbox(int _x, int _y, int _weight, int _height, std::string _value) : base(_x,_y,_weight,_height,_value) {}

    virtual void draw() {

        if(x >= 0 && x < X && y >= 0 && y < Y){
            setColor();
            gout << move_to(x,y) << box(weight,height) << color(0,0,0) << move_to(x,y) << box(weight,height-1);
            setColor();
            gout << move_to(x+5,y+height/2+5) << text(value);
        }

    }

    virtual void setFocus(int ex, int ey) {}

    virtual void setActive() {}

};

class writebox : public base {

protected:

    bool cursor;
    int ct,space;

public:

    writebox(int _x, int _y, int _weight, int _height, std::string _value) : base(_x,_y,_weight,_height,_value), cursor(false), ct(0), space(0) {}

    virtual void draw() {

        if(x >= 0 && x < X && y >= 0 && y < Y){

            while(gout.twidth(value.substr(space)) > weight) space++;
            while(space != 0 && gout.twidth(value.substr(space-1)) < weight) space--;

            setColor();
            gout << move_to(x,y) << box(weight,height) << color(0,0,0) << move_to(x+1,y+1) << box(weight-2,height-2);
            setColor();
            gout << move_to(x+5,y+height/2+5) << text(value.substr(space, value.size()));

            if(active){
                ct++;
                if(ct%25 == 0){
                    cursor = !cursor;
                    ct = 0;
                }
                if(cursor) gout << line(0,-10);
            }
        }

    }

    virtual void setActive() {

        if(focus) active = true;
        else active = false;

    }

    virtual void setEvent(event e) {

        if(e.type == ev_timer) draw();

        if(e.type == ev_mouse) setFocus(e.pos_x,e.pos_y);

        if(e.button == btn_left) setActive();

        if(e.keycode > 31 && e.keycode < 127) value += e.keycode;

        if(e.keycode == 8) value = value.substr(0,value.size()-1);

    }

};

class digitbox : public base {

protected:

    bool focus_up,focus_down;

public:

    digitbox(int _x, int _y, int _value) : base(_x,_y,75,25,convertIntToString(_value)), focus_up(false), focus_down(false) {}

    void draw(){

        if(x >= 0 && x < X && y >= 0 && y < Y){

            setColor();

            gout << move_to(x,y) << box(weight,height) << color(0,0,0) << move_to(x+1,y+1) << box(weight-2,height-2);

            if(focus_down) gout << move_to(x+1,y+1) << color(255,165,0) << box(weight/3-2,height-2);
            if(focus_up) gout << move_to(x+1+2*weight/3,y+1) << color(255,165,0) << box(weight/3-2,height-2);

            setColor();

            gout << move_to(x+8,y+height/2+5) << text("<") << move_to(x+weight/3+8,y+height/2+5) << text(value) << move_to(x+2*weight/3+8,y+height/2+5) << text(">");
        }
    }

    void setFocus(int ex, int ey) {

        if(ex >= x && ey >= y && ex < x+weight && ey < y+height ) focus = true;
        else focus = false;

        if( x < ex && x+weight/3 > ex && y < ey && y+height > ey ) focus_down = true;
        else focus_down = false;

        if( x+2*weight/3 < ex && x+weight > ex && y < ey && y+height > ey) focus_up = true;
        else focus_up = false;

    }

    void setActive() {

        if( focus_down ) value = convertIntToString(convertStringToInt(value)-1);
        if( focus_up ) value = convertIntToString(convertStringToInt(value)+1);

        if( convertStringToInt(value) < 0 ) value = convertIntToString(0);
        if( convertStringToInt(value) > 9 ) value = convertIntToString(9);

    }

    int getValue() {return convertStringToInt(value);}

};

class block : public base {

protected:

    int id, w, h;
    bool mterm;

public:

    block(int _x, int _y, int _size, int _id, int _w, int _h) : base((_x+_w*30),(_y+_h*30),30,30,convertIntToString(_id)), id(_id), w(_w), h(_h), mterm(false) {}

    int getW() const {return w;}
    int getH() const {return h;}
    int getID() const {return id;}

    std::list<int> getBinaryCode() {return convertIntToBinary(id);}
    int getBinWeight() {return getBinaryWeight(getBinaryCode());}

    virtual void setActive() {

        if(focus) active = true;
        else active = false;

    }

    virtual void draw() {

        if(x >= 0 && x < X && y >= 0 && y < Y){
            setColor();
            gout << move_to(x,y) << box(weight,height) << color(0,0,0) << move_to(x+1,y+1) << box(weight-2,height-2);
            setColor();
            gout << move_to(x+2,y+height/2+5) << text(value);
        }

    }

    bool isActive() {return active;}

};

class table {

protected:

    int peek_x, peek_y, block_size, input, w, h;

    std::string inputs = "#ABCDEFGHI";
    std::vector< block * > blocks;
    std::vector< std::vector<int> > sign_ver;
    std::vector< std::vector<int> > sign_hor;

public:

    table ( int _peek_x, int _peek_y, int _block_size ) : peek_x(_peek_x),peek_y(_peek_y),block_size(_block_size),input(0) {
        update(input);
    }

    void draw() {

        if(input != 0) gout << move_to(peek_x-1,peek_y-1) << color(100,100,100) << box(w*block_size+2,h*block_size+2);

        for(size_t i = 0; i < blocks.size(); i++) blocks[i]->draw();

        // kivalasztott block megkeresése

        int index = -1;
        for(size_t i = 0; i < blocks.size(); i++)
            if(blocks[i]->isActive()) {
                index = i;
                break;
            }

        //függöleges jelölés

        for(size_t i = 0; i < sign_ver.size(); i++){
            for(size_t j = 0; j < sign_ver[i].size(); j++){
                if(sign_ver[i][j] != 0) {
                    if(index > -1 && i == blocks[index]->getH()) gout << move_to(peek_x-j*block_size,peek_y+i*block_size) << color(20,20,20) << box(block_size,block_size) << move_to(peek_x-j*block_size,peek_y+i*block_size+1) << color(0,0,0) << box(block_size,block_size-1) << move_to(peek_x-j*block_size+1,peek_y+i*block_size+1) << color(255,165,0) << box(block_size,block_size) << move_to(peek_x-j*block_size+8,peek_y+i*block_size+18) << color(40,40,40) << text(inputs[sign_ver[i][j]]);
                    else gout << move_to(peek_x-j*block_size,peek_y+i*block_size) << color(100,100,100) << box(block_size,block_size) << move_to(peek_x-j*block_size,peek_y+i*block_size+1) << color(0,0,0) << box(block_size,block_size-1) << move_to(peek_x-j*block_size+8,peek_y+i*block_size+18) << color(180,180,180) << text(inputs[sign_ver[i][j]]);
                }
            }
        }

        //viszintes jelölés

        for(size_t i = 0; i < sign_hor.size(); i++){
            for(size_t j = 0; j < sign_hor[i].size(); j++){
                if(sign_hor[i][j] != 0) {
                    if(index > -1 && i == blocks[index]->getW()) gout << move_to(peek_x+i*block_size,peek_y-(j+1)*block_size) << color(20,20,20) << box(block_size,block_size) << move_to(peek_x+i*block_size+1,peek_y-(j+1)*block_size) << color(0,0,0) << box(block_size-1,block_size) << move_to(peek_x+i*block_size+1,peek_y-(j+1)*block_size+1) << color(255,165,0) << box(block_size,block_size-1) << move_to(peek_x+i*block_size+8,peek_y-(j+1)*block_size+18) << color(40,40,40) << text(inputs[sign_hor[i][j]]);
                    else gout << move_to(peek_x+i*block_size,peek_y-(j+1)*block_size) << color(100,100,100) << box(block_size,block_size) << move_to(peek_x+i*block_size+1,peek_y-(j+1)*block_size) << color(0,0,0) << box(block_size-1,block_size) << move_to(peek_x+i*block_size+8,peek_y-(j+1)*block_size+18) << color(180,180,180) << text(inputs[sign_hor[i][j]]);
                }
            }
        }
    }

    void update( int in ) {

        if(in == 0) {
            w=0;
            h=0;
        }
        else {
            w=pow(2,(in+1)/2);
            h=pow(2,in/2);
        }

        while(input > in) {
            input--;
            if(input == 0) blocks.clear();
            else split();
        }

        while(input < in) {
            input++;
            if(input == 1) {
                blocks.push_back(new block(peek_x,peek_y,block_size,0,0,0));
                blocks.push_back(new block(peek_x,peek_y,block_size,1,1,0));
            }
            else if(input%2==0) mirror(false);
            else if(input%2==1) mirror(true);
        }

        signer();

    }

    void mirror(bool right) {

        int s = blocks.size();
        for(size_t i = 0; i < s; i++) {
            if(right) blocks.push_back(new block(peek_x,peek_y,block_size,i+s,w-1-blocks[i]->getW(),blocks[i]->getH()));
            else blocks.push_back(new block(peek_x,peek_y,block_size,i+s,blocks[i]->getW(),h-1-blocks[i]->getH()));
        }
    }

    void signer() {

        sign_ver.clear();
        sign_hor.clear();

        int k = 0;

        while ( k < input) {

            k++;

            if(k == 1) {
                std::vector<int> sign_help;

                sign_help.push_back(0);

                sign_ver.push_back(sign_help);
                sign_hor.push_back(sign_help);

                sign_help.clear();

                sign_help.push_back(1);

                sign_hor.push_back(sign_help);
            }
            else if(k%2==1) {
                int s = sign_hor.size();
                for(size_t i = 0; i < s; i++){
                    sign_hor.push_back(sign_hor[s-1-i]);
                }
                for(size_t i = 0; i < sign_hor.size(); i++){
                    if(i < sign_hor.size()/2 ) sign_hor[i].push_back(0);
                    else sign_hor[i].push_back(k);
                }
            }
            else if(k%2==0){
                int s = sign_ver.size();
                for(size_t i = 0; i < s; i++){
                    sign_ver.push_back(sign_ver[s-1-i]);
                }
                for(size_t i = 0; i < sign_ver.size(); i++){
                    if(i < sign_ver.size()/2 ) sign_ver[i].push_back(0);
                    else sign_ver[i].push_back(k);
                }
            }
        }
    }

    void split() {
        int s = blocks.size();
        for(size_t i = s; i > s/2; i--) {
            block * b = blocks[i-1];
            blocks.pop_back();
            delete b;
        }
    }

    void setFocus(int ex, int ey) {
        for(size_t i = 0; i < blocks.size(); i++) {
            blocks[i]->setFocus(ex,ey);
        }
    }

    void setActive() {
        for(size_t i = 0; i < blocks.size(); i++) {
            blocks[i]->setActive();
        }
    }

    void setEvent(event e, int ins) {

        if(e.type == ev_timer) draw();

        if(e.type == ev_mouse) setFocus(e.pos_x,e.pos_y);

        if(e.button == btn_left) {
            update(ins);
            setActive();
        }

    }
};

class part : public base {

protected:

    int N,W,S,E;
    bool node;

public:

    part(int _x, int _y, std::string _value, int _N, int _W, int _S, int _E) : base(_x,_y,6,6,_value), N(_N), W(_W), S(_S), E(_E), node(false) {}

    part(int _x, int _y, std::string _value) : part(_x,_y,_value,0,0,0,0) {}

    virtual void draw() {

        if(x >= 0 && x < X && y >= 0 && y < Y){

            gout << move_to(x+2,y+2) << box(2,2);

            if(N>0)gout << move_to(x+2,y+2) << box(2,-N);
            if(W>0)gout << move_to(x+2,y+2) << box(-W,2);
            if(S>0)gout << move_to(x+2,y+4) << box(2,S);
            if(E>0)gout << move_to(x+4,y+2) << box(E,2);

            if(node) {
                if(focus) gout << color(255,165,0) << move_to(x+1,y+1) << box(4,4) << color(155,65,0);
                else if(active) gout << color(255,200,0) << move_to(x+1,y+1) << box(4,4) << color(255,165,0);
                else gout << color(200,200,200) << move_to(x+1,y+1) << box(4,4) << color(100,100,100);
            }

        }
    }

    virtual void setActive() {
        if(focus) active = true;
        else active = false;
    }

    void setNorth(int length) {N = length;}
    void setWest(int length) {W = length;}
    void setSouth(int length) {S = length;}
    void setEast(int length) {E = length;}

    void setNode(bool on) {node = on;}

    bool isIn() {return W > 0;}
    bool isOut() {return E > 0;}
    bool isNode() {return node;}

    int getX() {return x;}
    int getY() {return y;}
};

class wire : public base {

protected:

    int length, first, last;
    std::vector<part *> line;

public:

    wire(int _x, int _y, int _length, std::string _id) : base(_x,_y,6,_length*6,_id), length(_length), first(0), last(length-1)  {
        for(int i = 0; i < length; i++) line.push_back(new part(x,y+i*6,_id,3,0,3,0));
    }

    virtual void draw () {
        for(size_t i = first; i < last+1; i++) {
            setColor();
            line[i]->draw();
        }
    }

    virtual void setActive () {
        for(size_t i = first; i < last+1; i++) line[i]->draw();
    }

    void setPartIn(int loc, int s) {
        line[loc]->setWest(s);
        line[loc]->setNode(true);
    }
    void setPartOut(int loc, int s) {
        line[loc]->setEast(s);
        line[loc]->setNode(true);
    }
    void setFirst() {
        for(int i = 0; i < length; i++) {
            if(line[i]->isIn() || line[i]->isOut()) {
                first = i;
                break;
            }
        }
        line[first]->setNorth(0);
        line[first]->setNode(false);
    }
    void setLast() {
        for(int i = 0; i < length; i++) {
            if(line[i]->isIn() || line[i]->isOut()) {
                last = i;
            }
        }
        line[last]->setSouth(0);
        line[last]->setNode(false);
    }
};

class input : public base {

protected:

    bool out, outneg;

public:

    input(int _x, int _y, std::string _name) : base(_x,_y,30,30,_name), out(false), outneg(false){}

    virtual void draw() {

        if(x >= 0 && x < X && y >= 0 && y < Y){
            setColor();
            gout << move_to(x,y) << box(weight,height) << color(0,0,0) << move_to(x+1,y+1) << box(weight-2,height-2);
            setColor();
            gout << move_to(x+5,y+height/2+5) << text(value);

            if(outneg) gout << move_to(x+8,y+height) << box(2,16) << box(28,-2) << line(0,-15) << line(30,15) << move(-29,1) << line(0,15) << line(30,-15);

        }
    }

    virtual void setActive () {
        if(focus) active = true;
        else active = false;
    }

    void setOut(bool _out) {out = _out;}
    void setOutNeg(bool _outneg) {outneg = _outneg;}

    bool isOut() {return out;}
    bool isOutNeg() {return outneg;}
};

class andgate : public base {

protected:

    int fp;
    std::vector<std::string> ins;

public:

    andgate(int _x, int _y, int _part, std::string _in) : base(_x,_y,60,_in.length()*6+24,_in),fp(_part) {

        int pos = 0;
        while(pos < _in.size()-1) {
            ins.push_back(_in.substr(pos,2));
            pos += 2;
        }
    }

    virtual void draw() {

        setColor();
        gout << move_to(x+12,y) << line(30,0) << line(12,(height-18)/2) << line(0,3) << line(-12,(height-18)/2) << line(-31,0) << line(0,-height+18);

    }

    virtual void setActive() {}

    std::vector<std::string> getInputNames() {return ins;}

    int getPart() {return fp;}

};

class orgate : public base {

protected:

    std::vector<std::string> ins;

public:

    orgate(int _x, int _y, int _part) : base(_x,_y,60,_part*12+6,"") {}

    virtual void draw() {

        setColor();
        gout << move_to(x+12,y) << line(31,0) << line(12,height/2) << line(0,2) << line(-12,height/2) << line(-31,0) << line(12,-height/2) << line(0,-2) << line(-12,-height/2);

    }

    virtual void setActive() {}

    void addInput(std::string str) {
        value += str;
        ins.push_back(str);
    }

    std::vector<std::string> getInputNames() {return ins;}
};

class circuit {

protected:

    int x, y, partsize, alip;

    std::vector<input *> ins;
    std::vector<wire *> wis;
    std::vector<wire *> wis2;
    std::vector<andgate *> ags;

    orgate * og;

public:

    circuit(int _x, int _y):x(_x),y(_y),partsize(0),alip(1) {}

    void addWire(std::string name, bool inor) {

        if(inor) ;
        else ;

    }

    void addAndGate(std::string name) {
        ags.push_back(new andgate(x+90+wis.size()*6,y+alip*6,alip+1,name));
        alip += name.length()+4;

    }

    void addOrGate() {

        og = new orgate(x+90+wis.size()*6+60+wis2.size()*6,y+6,ags.size());

    }

    void addInput(std::string name, std::string type) {

        for(size_t i = 0; i < ins.size(); i++) {
            if(ins[i]->getValue() == name) {
                if(type == "0") ins[i]->setOutNeg(true);
                if(type == "1") ins[i]->setOut(true);
                return;
            }
        }

        ins.push_back( new input(x,y+ins.size()*60,name));
        partsize += 10;

        if(type == "0") ins[ins.size()-1]->setOutNeg(true);
        if(type == "1") ins[ins.size()-1]->setOut(true);

    }

    int getWireFromName(std::string str) {
        for(size_t i = 0; i < wis.size(); i++)
            if(str == wis[i]->getValue()) return i;

        return -1;
    }

    int getWire2FromName(std::string str) {
        for(size_t i = 0; i < wis2.size(); i++)
            if(str == wis2[i]->getValue()) return i;

        return -1;
    }


    void setUp(std::string str) {

        for(size_t i = 0; i < str.length(); i++)
            if(str[i] != '+' && str[i] != '1' && str[i] != '0') addInput(str.substr(i,1),str.substr(i-1,1));

        for(size_t i = 0; i < ins.size(); i++) {
            if(ins[i]->isOut()) {
                wis.push_back(new wire(x+90+wis.size()*6,y,partsize,"1"+ins[i]->getValue()));
                wis[wis.size()-1]->setPartIn(i*10+1,wis.size()*6+58);
            }
            if(ins[i]->isOutNeg()) {
                wis.push_back(new wire(x+90+wis.size()*6,y,partsize,"0"+ins[i]->getValue()));
                wis[wis.size()-1]->setPartIn(i*10+7,wis.size()*6+22);
            }
        }

        int p = getCharPos('+',str);
        while(p != -1) {
            addAndGate(str.substr(0,p));
            str = str.substr(p+1);
            p = getCharPos('+',str);
        }
        addAndGate(str);

        for(size_t i = 0; i < ags.size(); i++) {
            std::vector<std::string> vs = ags[i]->getInputNames();

            wis2.push_back(new wire(x+150+wis.size()*6+wis2.size()*6,y,partsize,ags[i]->getValue()));
            wis2[wis2.size()-1]->setPartIn(ags[i]->getPart()+vs.size()-1,wis2.size()*6+4);
            wis2[wis2.size()-1]->setPartOut(wis2.size()*2,(ags.size()-wis2.size())*6+18);

            for(size_t j = 0; j < vs.size(); j++) {
                wis[getWireFromName(vs[j])]->setPartOut(ags[i]->getPart()+j*2,(wis.size()-getWireFromName(vs[j]))*6+8);
            }
        }

        addOrGate();

        for(size_t i = 0; i < wis.size(); i++) {
            wis[i]->setFirst();
            wis[i]->setLast();
        }
        for(size_t i = 0; i < wis2.size(); i++) {
            wis2[i]->setFirst();
            wis2[i]->setLast();
        }
    }

    void draw() {
        for(size_t i=0; i < ins.size(); i++) ins[i]->draw();
        for(size_t i=0; i < wis.size(); i++) wis[i]->draw();
        for(size_t i=0; i < wis2.size(); i++) wis2[i]->draw();
        for(size_t i=0; i < ags.size(); i++) ags[i]->draw();
        og->draw();
    }

    void setEvent(event e) {
        if(e.type == ev_timer) draw();
    }
};

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

int bw_count(std::list<int> bin){

    int bin_weight = 0;

    for (std::list<int>::iterator it = bin.begin(); it != bin.end(); it++){
    if(*it == 1) bin_weight++; }


    std::cout<<bin_weight;
    return bin_weight;

    }

struct minterm{

    int mt;

    std::list<int> bin = convert_to_binary(mt); //ne felejtsd hogy itt forditva kell majd kiolvasni a helyiértékek miatt!

    int bin_weight = bw_count(convert_to_binary(mt));

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
            j++;
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

std::vector<std::vector<minterm> > old_implicants; //ebben lesznek eltárolva az eredeti inputok azon regiszterek decimális sorszámai melyek 1-esek az egyes mintermek avagy implikánsok is külön külön mégha csak egy tagúak akkor is vektorba vannak megadva

std::vector<std::vector<minterm> > new_implicants; //ebbe pushbackeli majd a progi az uj implikánsokat amik szomszédosak

void make_new_implicant_2(std::vector<std::vector<minterm> > old, std::vector<std::vector<minterm> > neew){


    for(int k = 0; k < old.size(); k++){
        for(int l = k+1; l < old[k].size()-k; l++){
        neew.push_back(make_new_implicant(old[k],old[l]));
        }
    }

}

int main()
{
    gout.open(1600,900);

    event ev;

    base * TB01 = new textbox(200,50,150,25,"Bemenetek száma:");
    base * DB01 = new digitbox(350,50,0);
    base * TB02 = new textbox(500,50,150,25,"Regiszterek:");
    base * WB01 = new writebox(650,50,150,25,"");

    table T(200,300,30);

    circuit C(1200,300);
    C.setUp("1B1D0E+0A0C0D0E+1A1C1E0F+1A1B1C0D0F");

    std::vector<std::vector<int> > prime_implicants;

    gin.timer(20);

    while(gin >> ev) {

        if(ev.type == ev_timer) gout << move_to(0,0) << color(0,0,0) << box(1600,900);

        TB01->setEvent(ev);
        WB01->setEvent(ev);
        TB02->setEvent(ev);
        DB01->setEvent(ev);

        C.setEvent(ev);

        T.setEvent(ev,convertStringToInt(DB01->getValue()));

        gout << refresh;

    }
    return 0;
};
