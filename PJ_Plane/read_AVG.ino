float avgRead(ReadFunc func, int n) // avgRead(ชือฟังชั้นที่จะเฉลี่ย, จำนวลsample);
{
    float sum = 0.0f;

    for (int i = 0; i < n; i++) {
        sum += func();
    }

    return sum / n;
}

float avgRead_Pin(ReadFuncPin func_Pin, int n) //avgRead_Pin(ชื่อฟังชั่นที่จะเฉลี่ล, Pinที่อ่าน, จำนวลsample);
{
    float sum = 0.0f;

    for (int i = 0; i < n; i++){
        sum += func_Pin();
    }

    return sum / n;
    
}
