#include "json_c.h"
#include <stdio.h>

int main() {
#ifdef _WIN32
    #include <windows.h>
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    ast_node_t* json = json_load_file("ejemplo.json");
    if (!json) {
        puts("ejemplo.json no encontrado");
        return 1;
    }

    printf("name: %s\n", json_get_string(json, "name"));
    printf("city: %s\n", json_get_string(json, "[\"address/address\"].city"));
    printf("Primer grade: %.2f\n", json_get_number(json, "grades[0]"));
    printf("Segundo proyecto: %s\n", json_get_string(json, "projects[1].title"));

    // Recorrer proyectos
    ast_node_t* projects = json_get_node(json, "projects");
    if (projects && projects->ramas) {
        for (size_t i = 0; i < size_a(projects->ramas); ++i) {
            char path[64];
            snprintf(path, sizeof(path), "projects[%zu].title", i);
            printf("Project %zu: %s (score: %.0f)\n", i,
                json_get_string(json, path),
                json_get_number(json, (snprintf(path, sizeof(path), "projects[%zu].score", i), path)));
        }
    }


    json_free(json);
    puts("exit");
    return 0;
}
