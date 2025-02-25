# Magia Editor

## Introdução
O Magia Editor é um ambiente de desenvolvimento integrado (IDE) especializado em Lua, projetado especificamente para integração com aplicações Qt Widgets em C++. Este projeto tem como objetivo principal facilitar a inclusão de um ambiente de scripting Lua robusto e eficiente em qualquer projeto Qt C++. Com um foco no design atraente e na usabilidade, o Magia Editor oferece uma experiência de usuário agradável e intuitiva.

Entre suas principais características, destaca-se a implementação de ferramentas de debug e step-over integradas, permitindo aos desenvolvedores depurar scripts Lua de maneira eficaz diretamente no editor. Além disso, o editor vem equipado com um console de output, enriquecendo ainda mais as funcionalidades disponíveis para os usuários.

O Magia Editor se diferencia por ser uma solução completa para scripting e depuração em Lua, apresentando uma interface elegante e uma integração perfeita com o ecossistema Qt. Seja para desenvolvimento de jogos, aplicações de desktop ou qualquer outro projeto que se beneficie de scripting Lua.

## Requisitos

- Qt versão 5 ou superior
- Compilador C++ compatível com o padrão C++17

## Instalação
Basta clonar o repositório e instalar as dependências com o script. Por exemplo:

```bash
git clone https://github.com/tatolevicz/qwidget-lua-editor.git .
cd MagiaEditor
./install_dependencies.sh
```

## Uso
Após criar um projeto no Qt Creator, adicione o Magia Editor no projeto. Por exemplo no CmakeLists.txt do projeto adicione:

``` 
set(PROJECT_SOURCES
    # Dont forget to add this to see the icons on editor
    MagiaEditor/EditorRes.qrc
```

e link as libs necessárias no editor:

```
add_subdirectory(MagiaEditor)

target_include_directories(${PROJECT_NAME} PUBLIC
    MagiaEditor/include
)

# This policy is needed to link the qt libraries needed against the editor
cmake_policy(SET CMP0079 NEW)
target_link_libraries(mg_editor PUBLIC
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Svg
)
#--------------------------------------------------------------

# Now link the mg_editor to your project 
target_link_libraries(${PROJECT_NAME}
    PUBLIC
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Svg
    mg_editor
)
```


Enjoy it!