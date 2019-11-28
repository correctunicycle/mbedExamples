#include "mbed.h"

/*write a program that flashes one of the coloured LEDs at a rate 
determined by potentiometer position. Frequencey increases 
as it is turned to the right. Linear variation of frequency is required
between the 2 extremes.*/


class LED {

protected:
 DigitalOut outputSignal;

public:
 LED(PinName pin) : outputSignal(pin) {}

void on(void) {
    outputSignal = 0;
}

void off(void) {
    outputSignal = 1;
}

void toggle(void) {
        if (outputSignal.read()){ //read returns current output value.
            outputSignal = 0;
        }
        else {
            outputSignal = 1;
        }
    }

int status(void) {return outputSignal.read();}

};

class Potentiometer{

private:
AnalogIn inputSignal;
float VDD, currentSamplePC, currentSampleVolts;

public:
        Potentiometer(PinName pin, float v) : inputSignal(pin), VDD(v) {}

        float amplitudeVolts(void) { 
             return (inputSignal.read()*VDD);
        }

        float amplitudePC(void) {
            return inputSignal.read();
        }  
        void sample(void){
                currentSamplePC = inputSignal.read();
                currentSampleVolts = currentSamplePC * VDD;
        }
        float getCurrentSampleVolts(void){
            return currentSampleVolts;
        }
        float getCurrentSamplePC(void){
            return currentSamplePC;
        }
};
class flashRate{
    private:
        Potentiometer* pot;
        LED* light;
        //left right sample and frequency balues for linear relationship.
        const float smin, smax, fmin, fmax;
        float gdt, incpt, period;
        Timeout to;
        //calculate gradient and intercept of linear relationship
        void set_gdt(void);
        void set_incpt(void);
    public:
    flashRate(Potentiometer* p ,LED* l,float xmn,
              float xmx, float ymn, float ymx): pot(p),light(l), smin(xmn),smax(xmx),
              fmin(ymn),fmax(ymx){
                set_gdt();
                set_incpt();
                to.attach(callback(this, &flashRate::updateRate), 0.01);
              }
    void updateRate(void);
};

void flashRate::set_gdt(void){
    gdt = (fmax-fmin)/(smax-smin);
}
void flashRate::set_incpt(void){
    incpt = fmin;
}

void flashRate::updateRate(void){
    light->toggle();
    period = 1.0f/((gdt*pot->getCurrentSampleVolts())+ incpt);
    to.attach(callback(this, &flashRate::updateRate), (period/2.0f));   

}

class pushButton{
    private:
        InterruptIn buttonState;
        int state;
        int counter;
    public:
        pushButton(PinName pin): buttonState(pin){
                //this class interrupts on rise and fall and uses a counter to six to enable the use of three different states
                counter = 0;
                buttonState.rise(callback(this,&pushButton::pressed));
                buttonState.fall(callback(this,&pushButton::released));
        };
        void pressed(void){
            counter++;
            if(counter == 6){
                counter = 0;
            }
        }
        void released(void){
            counter++;
             if(counter == 6){
                counter = 0;
            }
        }
        
        int getcounter(void){
            return counter;
        }


};
//reads state of pushbutton and turns relevant leds on.
class control{
    private:
    Potentiometer* pot;
    pushButton* p;
    public:
    control(Potentiometer* pp ,pushButton* push):pot(pp),p(push){
    }
  
    void state2(){
        LED* redLED = new LED(D5);
        flashRate* flashy= new flashRate(pot,redLED,0,3.3,1,10);
        while(p->getcounter() % 2 == 0){
            pot->sample();
            wait(0.01);
        }
        delete flashy;
        redLED->off();
        delete redLED;
    }
    void state1(){
        LED* blueLED= new LED(D8);
        flashRate* flashy1= new flashRate(pot,blueLED,0,3.3,1,10);
        while(p->getcounter() % 2 == 0){
            pot->sample();
            wait(0.01);
        }
        delete flashy1;
        blueLED->off();
        delete blueLED;
    }
    void state3(){
        LED* greenLED= new LED(D9);
        flashRate* flashy1= new flashRate(pot,greenLED,0,3.3,1,10);
        while(p->getcounter() % 2 == 0){
            pot->sample();
            wait(0.01);
        }
        delete flashy1;
        greenLED->off();
        delete greenLED;
    }
    void branch(){
        if(p->getcounter() % 2 == 0){
          if(p->getcounter() == 0){
              state1();
        }
            if(p->getcounter() == 2){
              state2();
        }
        if(p->getcounter() == 4){
              state3();
        }
        }
    }



};



int main(void) {
    //creates necessary objects then infnitely calls the branch function of work
    Potentiometer* leftHand= new Potentiometer(A0,3.3);
    //flashRate flashy =  new flashRate(leftHand,redLED,0,3.3,1,10);
    pushButton* newpush = new pushButton(D4);
    control work(leftHand,newpush);
    while(true){
        work.branch();
    }
}

