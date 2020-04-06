#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFSIZE 1000

/*
useful references:
https://en.wikipedia.org/wiki/Korean_language_and_computers#Hangul_in_Unicode
https://unicode.org/charts/
*/

char *decompose(char *co) {
	int co_i, de_i;
	int len = strlen(co);
	char *de = malloc(len * 3 + 1);
	if (de == NULL) return co;
	for (co_i = 0, de_i = 0; co_i < len - 2; co_i++, de_i++) {
		de[de_i] = co[co_i];
		if ((co[co_i] & 0xe0) != 0xe0 ||
			(co[co_i + 1] & 0x80) != 0x80 ||
			(co[co_i + 2] & 0x80) != 0x80) continue;
		int jamo;
		int syl = ((co[co_i] & 0x0f) << 12) |
			((co[co_i + 1] & 0x3f) << 6) |
			((co[co_i + 2] & 0x3f) << 0);
		if (syl < 0xac00 || syl > 0xd7a3) continue;
		co_i += 2;
		syl -= 0xac00;
		jamo = syl / 588 + 0x1100; // initial consonant
		de[de_i] = 0xe0 | ((jamo & (0x0f << 12)) >> 12);
		de[++de_i] = 0x80 | ((jamo & (0x3f << 6)) >> 6);
		de[++de_i] = 0x80 | ((jamo & (0x3f << 0)) >> 0);
		jamo = (syl % 588) / 28 + 0x1161; // medial vowel
		de[++de_i] = 0xe0 | ((jamo & (0x0f << 12)) >> 12);
		de[++de_i] = 0x80 | ((jamo & (0x3f << 6)) >> 6);
		de[++de_i] = 0x80 | ((jamo & (0x3f << 0)) >> 0);
		jamo = syl % 28 + 0x11a7; // final consonant
		if (jamo == 0x11a7) continue;
		de[++de_i] = 0xe0 | ((jamo & (0x0f << 12)) >> 12);
		de[++de_i] = 0x80 | ((jamo & (0x3f << 6)) >> 6);
		de[++de_i] = 0x80 | ((jamo & (0x3f << 0)) >> 0);
	}
	for (; co_i <= len; co_i++, de_i++) {
		de[de_i] = co[co_i];
	}
	return de;
}

char *combine(char *de) {
	static char co[BUFSIZE];
	int de_i, co_i;
	int len = strlen(de);
	for (de_i = 0, co_i = 0; de_i < len - 5; de_i++, co_i++) {
		int initial, medial, final = -1;
		int syl;
		co[co_i] = de[de_i];
		if ((de[de_i] & 0xe0) != 0xe0 ||
			(de[de_i + 1] & 0x80) != 0x80 ||
			(de[de_i + 2] & 0x80) != 0x80 ||
			(de[de_i + 3] & 0xe0) != 0xe0 ||
            (de[de_i + 4] & 0x80) != 0x80 ||
            (de[de_i + 5] & 0x80) != 0x80) continue;
		initial = (((de[de_i] & 0x0f) << 12) |
			((de[de_i + 1] & 0x3f) << 6) |
			((de[de_i + 2] & 0x3f) << 0)) - 0x1100;
		medial = (((de[de_i + 3] & 0x0f) << 12) |
			((de[de_i + 4] & 0x3f) << 6) |
			((de[de_i + 5] & 0x3f) << 0)) - 0x1161;
		if (initial < 0 || initial > 18 || medial < 0 || medial > 20) continue;
		syl = initial * 588 + medial * 28 + 0xac00;
		if (de_i < len - 8 &&
			(de[de_i + 6] & 0xe0) == 0xe0 &&
            (de[de_i + 7] & 0x80) == 0x80 &&
            (de[de_i + 8] & 0x80) == 0x80) {
			final = (((de[de_i + 6] & 0x0f) << 12) |
				((de[de_i + 7] & 0x3f) << 6) |
				((de[de_i + 8] & 0x3f) << 0)) - 0x11a7;
		}
		if (final >= 0 && final <= 27) {
			syl += final;
			de_i += 3;
		}
		de_i += 5;
		co[co_i] = 0xe0 | ((syl & (0x0f << 12)) >> 12);
		co[++co_i] = 0x80 | ((syl & (0x3f << 6)) >> 6);
		co[++co_i] = 0x80 | ((syl & (0x3f << 0)) >> 0);
	}
	for (; de_i <= len; de_i++, co_i++) {
		co[co_i] = de[de_i];
	}
	return co;
}

int main(int argc, char **argv) {
	int i = 1;
	if (argc > 1 && argv[1][0] == '-') {
		if (!strcmp("-c", argv[1])) {
			char buf[BUFSIZE];
			while (fgets(buf, BUFSIZE, stdin) != NULL) {
				printf("%s", combine(buf));
			}
			exit(0);
		} else if (!strcmp("--", argv[1])) {
			i++;
		} else {
			fprintf(stderr, "usage: %s text ... - convert text into the searchable (jamo-separated) form\n", argv[0]);
			fprintf(stderr, "         -c       - (reverse) combine standard input into the normalized form\n");
			fprintf(stderr, "         -h       - help\n");
			exit(0);
		}
	}
	for (; i < argc; i++) {
		char *s = decompose(argv[i]);
		printf("%s", s);
		free(s);
		if (i != argc - 1) {
			printf(" ");
		}
	}
	return 0;
}
