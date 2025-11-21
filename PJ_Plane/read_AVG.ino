float avgRead(ReadFunc func, int n) // avgRead(ชือฟังชั้นที่จะเฉลี่ย, จำนวลsample);
{
    float sum = 0.0f;

    for (int i = 0; i < n; i++) {
        sum += func();
    }

    return sum / n;
}
