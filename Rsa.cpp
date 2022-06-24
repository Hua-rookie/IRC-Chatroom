#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <time.h>
#include <string.h>
#include <algorithm>
#include <string>
using namespace std;

long long int powerMod(long long int pa, long long int pd, long long int pn){
	long long int r;
	long long int i;
	long long int tmp = 0;

	r = (long long int) (log(abs(pd))/log(2)) + 1;
	int *bin = new int[r];
	long long int *dec = new long long int[r];

	tmp = pd;
	//cout << "bin: ";
	for (i = 0; i < r; i++){
		bin[r-i-1] = tmp % 2;
		//cout << bin[r-i-1];
		tmp = (long long int) (tmp / 2);
	}
//  cout << endl;
	dec[0] = pa;
	for (i = 1; i < r; i++){
		dec[i] = (long long int)((dec[i-1]*dec[i-1] * pow(pa,bin[i]))) % pn;
	//	cout << dec[i-1] << "*" << dec[i-1] << "*" << pa << "^" << bin[i] << "%" << pn <<endl;
	//	cout << dec[i] << " " << endl;
	}

	return dec[r-1];
}

bool prost(long long int n) {
	bool k = true;
	if (n != 2){
		for (int i = 2; i < (int) sqrt(n) + 1; i++){
			if (n % i == 0){
				k = false;
				break;
			}
		}
	}
	return k;
}

int genProst(){
	int t = 10 + rand() % 90;

	do{
		t++;
	} while (!prost(t));

	return t;
}

int evklid(long long int num1, long long int num2){
	vector<int> r;
	int i=1;
	r.push_back(max(num1,num2));
	r.push_back(min(num1,num2));
	while (r[i] != 0) {
		i++;
		r.push_back(r[i-2] % r[i-1]);
	}
	return r[i-1];
}

int genCoprime(long long int x) {
	//ispolzuem chisla Ferma
	int i = 1 + rand() % 99;
	do {
		i++;
	} while (evklid(x,i) != 1);
	return i;
}

int gend(long long int f, long long int e) {
	//return (int) ((f * (1 + rand() % 4) + 1) / e);
	int i;
	for (i=1;i<100;i++) {
//		cout << ((f * (i) + 1) % e) <<endl;
		if ( ((f * (i) + 1) % e) == 0) {
			return (int) ((f * (i) + 1) / e);
		}
	}
	return 0;
}


int produce_keys(long long int* pub_key, long long int* pri_key, long long int* N) {
	srand(time(0));
	int p = genProst();
	int q = genProst();
	long long int f = (p - 1) * (q - 1);
	*N = p * q;
	*pub_key = genCoprime(f);
	*pri_key = gend(f, *pub_key);
//	*pub_key = 71;
//	*pri_key = 1655;
//	*N = 3589;
	if (pub_key == NULL || pri_key == NULL) {
		cout << "密钥生成失败" << endl;
		return -1;
	}
	else {
		cout << "密钥生成成功" << endl;
		cout << "公钥：" << *pub_key << endl;
		cout << "私钥：" << *pri_key << endl;
		cout << "大数：" << *N << endl;
		cout << endl;
		return 0;
	}
}

int Encrypte(char* ori, long long int* cry, long long int pub_key, long long int N) {
	if (ori == NULL) {
		cout << "待加密字符串无效" << endl;
		return -1;
	}
	if (cry == NULL) {
		//cry = new long long int[strlen(ori)];//sizeof待调试
	}
	
	char* p = ori;
	long long int* q = cry;
	while (*p != 0) {
		*q++ = powerMod(*p++, pub_key, N);
	}
	*q = 0;
	cout << "加密成功" << endl;
	cout << "密文：";
	for (q = cry; *q != 0; q++) {
		cout << *q << " ";
	}
	cout << endl;
	cout << "明文：" << ori << endl;
	cout << endl;
	return 0;
}

int Decrypte(long long int* cry, char* ori, long long int pri_key, long long int N) {
	if (cry == NULL) {
		cout << "待解密字符串无效" << endl;
		return -1;
	}
	if (ori == NULL) {
		//ori = new char[strlen(cry)];//sizeof待调试
	}
	long long int* p = cry;
	char* q = ori;
	int x = 0;
	while (*p != 0) {
		*q++ = powerMod(*p++, pri_key, N);
	}
	*q = 0;
	cout << "解密成功" << endl;
	cout << "密文：";
	for (p = cry; *p != 0; p++) {
		cout << *p << " ";
	}
	cout << endl;
	cout << "明文：" << ori << endl;
	cout << endl;
	return 0;
}

long long int num2bit(long long int src){
	long long int dst = 0;
	for (long long int i = 1; src != 0; src/=2, i*=10){
		dst += (src%2) * i;
	}
	return dst;
}
long long int bit2num(long long int src){
	long long int dst = 0;
	for (long long int i = 1; src != 0; src/=10, i*=2){
		dst += (src%10) * i;
	}
	return dst;
}
#define NUM 2
char* num2str(long long int src){
	char *dst = new char[sizeof(long long int)*NUM];
	dst[sizeof(long long int)*NUM] = 0;
	char *p = dst + strlen(dst) - 1;

	for ( ; src != 0; src/=10){
		*p-- = (src%10) + ('0'-0);
	}
	while((p+1) != dst){
		*p-- = '0';
//		if (p == dst) system("pause");
	}
	return dst;
}

long long int str2num(char* src){
	long long int dst = 0;
	char *p = src + strlen(src) - 1;

	for (long long int i = 1; (p+1) != src; i*=10){
		dst += ((*p--)-('0'-0)) * i;
	}
	return dst;
}

int Encrypte(char* ori, char *&cry, long long int pub_key, long long int N) {
	if (ori == NULL) {
		cout << "待加密字符串无效" << endl;
		return -1;
	}
	if (cry == NULL) {
		int len = strlen(ori)*sizeof(long long int);
		cry = new char[len];//sizeof待调试
		cry[len] = 0;
		cout << "已成功申请内存" << endl;
	}
	
	char* p = ori;
	char* tmp;// = new char[sizeof(long long int)];
	//tmp[sizeof(long long int)] = 0;
	char* q = cry;
	while (*p != 0) {
		tmp = num2str(num2bit(powerMod(*p++, pub_key, N)));
		//cout << *(p-1) << ":" << tmp << endl;
		for (char* w = tmp; *w != 0; ){
			*q++ = *w++;
		}
	}
	*q = 0;
	cout << "加密成功" << endl;
	cout << "密文：" << cry <<endl;
	cout << endl;
	cout << "明文：" << ori << endl;
	cout << endl;
	return 0;
}

int Decrypte(char* cry, char *&ori, long long int pri_key, long long int N) {
	if (cry == NULL) {
		cout << "待解密字符串无效" << endl;
		return -1;
	}
	if (ori == NULL) {
		int len = strlen(cry)/sizeof(long long int);
		ori = new char[len];
		ori[len] = 0;
	}
	char* p = cry;
	char* tmp = new char[sizeof(long long int)*NUM];
	tmp[sizeof(long long int)*NUM] = 0;
	char* q = ori;
	long long int cal = 0;
	int i = 0;
	
	while (*p != 0) {
		for (char* w = tmp, i=sizeof(long long int)*NUM; i > 0; i--){
			*w++ = *p++;
		}
		cal = bit2num(str2num(tmp));
		*q++ = powerMod(cal, pri_key, N);
		//cout << cal << ":" << *(q-1) << endl;
	}
	*q = 0;
	cout << "解密成功" << endl;
	cout << "密文：" << cry << endl;
	cout << endl;
	cout << "明文：" << ori << endl;
	cout << endl;
	return 0;
}

int main(){
	long long int pub_key, pri_key, N;
	produce_keys(&pub_key, &pri_key, &N);//生成公钥和私钥，仅向彼此传递公钥

	char *pub_key_c = num2str(pub_key);
	char *pri_key_c = num2str(pri_key);
	
	char *ori = (char*)"huaermohuaermohuaermo";
//	int len = strlen(ori) * sizeof(long long int);
//	char *cry = new char[len];//sizeof待调试
//	cry[len] = 0;
	char *cry = NULL;
	Encrypte(ori, cry, pub_key, N);
	if (cry == NULL) cout << "内存申请错误" << endl;
	char *res = NULL;	
	Decrypte(cry, res, pri_key, N);
	if (res == NULL) cout << "内存申请错误" << endl;
	system("pause");
}