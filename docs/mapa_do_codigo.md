Onde o programa começa:
	O programa começa com a Main, onde ele faz o parse_args, confirmando que é um argumento completo. Após isso envia para a função trace_program. 

Onde o processo alvo é criado:
	No trace_runtime, no lauch_tracee ele cria o filho que será o processo monitorado.

Onde o runtime chama o callback:
	Dentro do trace_runtime, no Trace_program há um loop infinito de "while(1)", onde ele fica esperando para ser parado.

Quais arquivos o grupo deve modificar:
	trace_runtime.c
	formatter.c
	pairer.c

qual TODO aparece primeiro ao executar o scaffold:
	TODO Semana 2: implementar launch_tracee()

Qual é a principal dúvida técnica do grupo nesse momento:
	O funcionamento do corpo do trace_runtime.
	A sincronização entre pai e filho, todavia esse será implementado na semana 2, dentro do .
	O comando ./toytrace -help está dando "erro: comando desconhecido: -help". Todos os outros testes funcionaram normalmente, vejo que esse programa não está implementado e acho que é um erro que deve acontecer; todavia ainda me deixou um pouco incomodado.