#include <stdlib.h>

const char* magic8ball(void) {
    const char *responses[] = {
        "It is certain.",
        "It is decidedly so.",
        "Without a doubt.",
        "Yes definitely.",
        "You may rely on it.",
        "As I see it, yes.",
        "Most likely.",
        "Outlook good.",
        "Yes.",
        "Signs point to yes.",
        "Reply hazy, try again.",
        "Ask again later.",
        "Better not tell you now.",
        "Cannot predict now.",
        "Concentrate and ask again.",
        "Don't count on it.",
        "My reply is no.",
        "My sources say no.",
        "Outlook not so good.",
        "Very doubtful."
    };

    int index = rand() % 20;
    return responses[index];
}

int genie_mode(){
  while(1) {
    // Ask for question input
    for(int i = 0; i < 25; i++) {
      delay_random_timing();
      jinn_noop();
      delay_random_timing();
      jinn_junk_inst();
      delay_random_timing();
    }
    magic8ball();
  }
}
