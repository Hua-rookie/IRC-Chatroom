#ifndef MY_AES_H
#define MY_AES_H

/**
 * ���� p: ���ĵ��ַ������顣
 * ���� plen: ���ĵĳ���,���ȱ���Ϊ16�ı�����
 * ���� key: ��Կ���ַ������顣
 */
void aes(char *p, int plen, char *key);

/**
 * ���� c: ���ĵ��ַ������顣
 * ���� clen: ���ĵĳ���,���ȱ���Ϊ16�ı�����
 * ���� key: ��Կ���ַ������顣
 */
void deAes(char *c, int clen, char *key);

#endif
