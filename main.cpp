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

class base {

protected:

    int x,y,weight,height;

    bool focus, active;

    std::string value;

public:

    base(int _x, int _y, int _weight, int _height, std::string _value) : x(_x), y(_y), weight(_weight), height(_height), focus(false), active(false), value(_value) {

        if(weight < 25) weight = 25;
        if(height < 25) height = 25;

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

    virtual void setActive() = 0;

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

        // kivalasztott block megkeres�se

        int index = -1;
        for(size_t i = 0; i < blocks.size(); i++)
            if(blocks[i]->isActive()) {
                index = i;
                break;
            }

        //f�gg�leges jel�l�s

        for(size_t i = 0; i < sign_ver.size(); i++){
            for(size_t j = 0; j < sign_ver[i].size(); j++){
                if(sign_ver[i][j] != 0) {
                    if(index > -1 && i == blocks[index]->getH()) gout << move_to(peek_x-j*block_size,peek_y+i*block_size) << color(20,20,20) << box(block_size,block_size) << move_to(peek_x-j*block_size,peek_y+i*block_size+1) << color(0,0,0) << box(block_size,block_size-1) << move_to(peek_x-j*block_size+1,peek_y+i*block_size+1) << color(255,165,0) << box(block_size,block_size) << move_to(peek_x-j*block_size+8,peek_y+i*block_size+18) << color(40,40,40) << text(inputs[sign_ver[i][j]]);
                    else gout << move_to(peek_x-j*block_size,peek_y+i*block_size) << color(100,100,100) << box(block_size,block_size) << move_to(peek_x-j*block_size,peek_y+i*block_size+1) << color(0,0,0) << box(block_size,block_size-1) << move_to(peek_x-j*block_size+8,peek_y+i*block_size+18) << color(180,180,180) << text(inputs[sign_ver[i][j]]);
                }
            }
        }

        //viszintes jel�l�s

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


class part {};

//class input : public part {};

class wire : public part {};

class andgate : public part {};

class orgate : public part {};

//class circuit {};




/*
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

    std::list<int> bin = convert_to_binary(mt); //ne felejtsd hogy itt forditva kell majd kiolvasni a helyi�rt�kek miatt!

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

std::vector<std::vector<minterm> > old_implicants; //ebben lesznek elt�rolva az eredeti inputok azon regiszterek decim�lis sorsz�mai melyek 1-esek az egyes mintermek avagy implik�nsok is k�l�n k�l�n m�gha csak egy tag�ak akkor is vektorba vannak megadva

std::vector<std::vector<minterm> > new_implicants; //ebbe pushbackeli majd a progi az uj implik�nsokat amik szomsz�dosak

void make_new_implicant_2(std::vector<std::vector<minterm> > old, std::vector<std::vector<minterm> > neew){


    for(int k = 0; k < old.size(); k++){
        for(int l = k+1; l < old[k].size()-k; l++){
        neew.push_back(make_new_implicant(old[k],old[l]));
        }
    }

}
*/

struct input {

    int p_x, p_y, in_n, id;

    char name;

    bool out, outneg;

    input(int _p_x, int _p_y, int _in_n, char _name):p_x(_p_x),p_y(_p_y),in_n(_in_n),id(_in_n),name(_name),out(false),outneg(false){}

    void draw() {

        gout << move_to(p_x,p_y) << color(255,165,0) << box(25,25) << move_to(p_x+1,p_y+1) << color(0,0,0) << box(23,23) << move_to(p_x+8,p_y+16) << color(255,165,0) << text(name);

        if(out) {

            gout << move_to(p_x+25,p_y+3) << color(255,165,0) << box(40+(id*2-1)*6-4,2);

            //wireminta
            gout << move(-1,1) << color(155,65,0) << box(4,-4) << move(-1,4) << color(255,165,0) << box(-2,((in_n*6)+50)*(in_n-id)+70) << move(0,-((in_n*6)+50)*(in_n-id)-74) << box(2,-((in_n*6)+50)*id+60);

        }
        if(outneg) {

            gout << move_to(p_x+3,p_y+25) << color(155,65,0) << box(2,in_n*6) << box(27,-2) << line(0,-15) << line(30,16) << line(0,1) << line(-30,15) << line(0,-16) << move(30,0) << box(5+id*2*6-4,2);

            //wireminta
            gout << move(-1,1) << color(255,165,0) << box(4,-4) << move(-1,4) << color(155,65,0) << box(-2,((in_n*6)+50)*(in_n-id)+10) << move(0,-((in_n*6)+50)*(in_n-id)-14) << box(2,-((in_n*6)+50)*id);
        }


    }

    void addIn() {in_n++;p_y = p_y+(id-1)*6;}
    void setOut(bool _out) {out = _out;}
    void setOutNeg(bool _outneg) {outneg = _outneg;}

};

struct circuit {

    int x, y;

    std::string input_name_string;

    std::vector<input *> ins;
    std::vector<wire *> wis;
    std::vector<andgate *> ags;
    std::vector<orgate *> ogs;

    circuit(int _x, int _y, std::string _input_name_string):x(_x),y(_y),input_name_string(_input_name_string) {}

    void addAndGate() {}
    void addOrGate() {}
    void addInput() {

        for(size_t i=0; i < ins.size(); i++) ins[i]->addIn();

        input * pi = new input(x,y+ins.size()*(50+ins.size()*6),ins.size()+1,input_name_string[ins.size()]);

        ins.push_back(pi);
    }

    void setUp() {}

    void draw() {

            for(size_t i=0; i < ins.size(); i++) ins[i]->draw();
    }

    void setOut(int _id) {for(size_t i=0; i < ins.size(); i++) if(ins[i]->id == _id) ins[i]->setOut(true);}
    void setOutNeg(int _id) {for(size_t i=0; i < ins.size(); i++) if(ins[i]->id == _id) ins[i]->setOutNeg(true);}

};

int main()
{


    gout.open(1600,900);

    event ev;

    base * TB01 = new textbox(200,50,150,25,"Bemenetek sz�ma:");
    base * DB01 = new digitbox(350,50,0);
    base * TB02 = new textbox(500,50,150,25,"Regiszterek:");
    base * WB01 = new writebox(650,50,150,25,"");

    table T(200,300,30);

/*
    circuit CI(1000,200,"ABCDEFGHI");
    CI.addInput();
    CI.addInput();
    CI.addInput();
    CI.addInput();
    CI.addInput();
    CI.addInput();
    CI.setOut(1);
    CI.setOut(2);
    CI.setOut(3);
    CI.setOut(4);
    CI.setOut(5);
    CI.setOut(6);
    CI.setOutNeg(1);
    CI.setOutNeg(2);
    CI.setOutNeg(3);
    CI.setOutNeg(4);
    CI.setOutNeg(5);
    CI.setOutNeg(6);

    std::vector<std::vector<int> > prime_implicants;
*/

    gin.timer(20);

    while(gin >> ev) {

        if(ev.type == ev_timer) gout << move_to(0,0) << color(0,0,0) << box(1600,900);

        TB01->setEvent(ev);
        WB01->setEvent(ev);
        TB02->setEvent(ev);
        DB01->setEvent(ev);

        T.setEvent(ev,convertStringToInt(DB01->getValue()));



        gout << refresh;

    }
    return 0;
};
