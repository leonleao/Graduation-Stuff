dumpintervalo (struct nodo *p, char *buf, int n, char *inicio, char *fim) {
	
	int i, letra;
	int leninicio = strlen (inicio);
	int lenfim = strlen (fim);
	
	if (p == NULL)
		return;
	if (n < leninicio && n < lenfim) {
		if (inicio[n] != fim [n]) {
			letra = (int)(fim[n] - inicio[n]);
			for (i=0; i<=letra; i++){
				buf[n] = inicio[n] + i;
				dumpintervalo (p->p[inicio[n]-'a'+i, buf, n+1, inicio, fim);
			}
		}
		else {
			buf[n] = inicio[n];
			dumpintervalo (p->p[inicio[n]-'a', buf, n+1, inicio, fim);
		}
	}
	else if (n == leninicio) {
			letra = fim[n] - 'a';
			if (p->flag) {
				buf[n] = '\0';
				printf("%s\n",buf);
			}
			for (i =0; i<=letra; i++) {
				buf[n] = 'a' - i;
				dumpintervalo (p->p['a'+i], buf, n+1, inicio, fim);
			}
		}
		else if (n == lenfim) {
				//terminar
			}
			else {
				if (p->flag) {
					buf[n] = '\0';
					printf("%s\n",buf);
				}
				for (i = 0; i<26; i++) {
					if (p->p[i] != NULL) {
						buf[n] = 'a' + i;
						dumpintervalo (p->p['a'+i], buf, n+1, inicio, fim);
					}
				
				}
				
			}
}
