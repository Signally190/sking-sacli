#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "tea.h"
#include "../systeminc/VMProtectSDK.h"



//long TEA解密(long* v, long n, long* k)
//{
//	unsigned long z,y,sum,e;
//	z=v[n-1];
//	y=v[0];
//	sum=0;
//	long p, q;
//	q = 6 + 52/n;
//	sum = q*DELTA;
//    while(sum != 0)
//	{
//		e = (sum >> 2) & 3;
//		for (p=n-1; p>0; p--)
//			z = v[p-1], y = v[p] -= MX;
//		z = v[n-1]; y = v[0] -= MX;
//		sum -= DELTA;
//	}
//	return 0;
//}
//
