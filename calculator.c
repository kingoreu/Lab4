#include <gtk/gtk.h>

GtkWidget *entry;  // 입력창을 전역 변수로 선언

// 버튼 클릭 시 처리 함수
void on_button_clicked(GtkButton *button, gpointer user_data) {
    const gchar *text = gtk_button_get_label(button);
    const gchar *entry_text = gtk_entry_get_text(GTK_ENTRY(entry));
    
    // 계산기 입력창에 버튼의 값을 추가
    gchar *new_text = g_strjoin("", entry_text, text, NULL);
    gtk_entry_set_text(GTK_ENTRY(entry), new_text);
    g_free(new_text);
}

// 'C' 버튼 클릭 시 입력창 초기화
void on_clear_clicked(GtkButton *button, gpointer user_data) {
    gtk_entry_set_text(GTK_ENTRY(entry), "");
}

// '=' 버튼 클릭 시 계산 결과 출력
void on_equal_clicked(GtkButton *button, gpointer user_data) {
    const gchar *text = gtk_entry_get_text(GTK_ENTRY(entry));
    if (text == NULL || *text == '\0') {
        gtk_entry_set_text(GTK_ENTRY(entry), "Error");
        return;  // 입력이 없으면 바로 종료
    }

    // 간단한 사칙 연산 처리 로직을 추가
    gdouble result = 0.0;
    gchar *endptr = NULL;

    // 여기에 연산자 처리 추가 (예: +, -, *, /)
    if (strchr(text, '+')) {
        gchar **tokens = g_strsplit(text, "+", -1);
        if (tokens[0] && tokens[1]) {
            result = atof(tokens[0]) + atof(tokens[1]);
        }
        g_strfreev(tokens);
    } else if (strchr(text, '-')) {
        gchar **tokens = g_strsplit(text, "-", -1);
        if (tokens[0] && tokens[1]) {
            result = atof(tokens[0]) - atof(tokens[1]);
        }
        g_strfreev(tokens);
    } else if (strchr(text, '*')) {
        gchar **tokens = g_strsplit(text, "*", -1);
        if (tokens[0] && tokens[1]) {
            result = atof(tokens[0]) * atof(tokens[1]);
        }
        g_strfreev(tokens);
    } else if (strchr(text, '/')) {
        gchar **tokens = g_strsplit(text, "/", -1);
        if (tokens[0] && tokens[1]) {
            result = atof(tokens[0]) / atof(tokens[1]);
        }
        g_strfreev(tokens);
    } else {
        // 만약 연산자가 없으면 숫자만 변환
        result = g_strtod(text, &endptr);  // 텍스트를 실수로 변환
        if (*endptr != '\0') {
            gtk_entry_set_text(GTK_ENTRY(entry), "Error");
            return;
        }
    }

    gchar *result_text = g_strdup_printf("%f", result); // 결과를 문자열로 포맷
    gtk_entry_set_text(GTK_ENTRY(entry), result_text);
    g_free(result_text);
}

int main(int argc, char *argv[]) {
    GtkWidget *window;
    GtkWidget *grid;
    GtkWidget *button;
    
    // GTK 초기화
    gtk_init(&argc, &argv);

    // 창 생성
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "계산기");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 400);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);

    // 그리드 생성
    grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    // 입력창 생성
    entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), entry, 0, 0, 4, 1);

    // 계산기 버튼 생성
    const gchar *buttons[4][4] = {
        {"7", "8", "9", "/"},
        {"4", "5", "6", "*"},
        {"1", "2", "3", "-"},
        {"C", "0", "=", "+"}
    };

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            button = gtk_button_new_with_label(buttons[i][j]);
            g_signal_connect(button, "clicked", G_CALLBACK(on_button_clicked), NULL);
            gtk_grid_attach(GTK_GRID(grid), button, j, i + 1, 1, 1);
        }
    }

    // 'C' 버튼 클릭 시 초기화
    GtkWidget *clear_button = gtk_button_new_with_label("C");
    g_signal_connect(clear_button, "clicked", G_CALLBACK(on_clear_clicked), NULL);
    gtk_grid_attach(GTK_GRID(grid), clear_button, 0, 5, 2, 1);

    // '=' 버튼 클릭 시 계산
    GtkWidget *equal_button = gtk_button_new_with_label("=");
    g_signal_connect(equal_button, "clicked", G_CALLBACK(on_equal_clicked), NULL);
    gtk_grid_attach(GTK_GRID(grid), equal_button, 2, 5, 2, 1);

    // 윈도우 "destroy" 시그널 처리
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // 윈도우 표시
    gtk_widget_show_all(window);

    // GTK 메인 루프 실행
    gtk_main();

    return 0;
}
