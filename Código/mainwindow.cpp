#include "mainwindow.h"

#include "funcoes.h"
#include "stb_image.h"
#include "stb_image_write.h"

#include <QAction>
#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDir>
#include <QDoubleSpinBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QKeySequence>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QResizeEvent>
#include <QStatusBar>
#include <QVBoxLayout>

#include <cstring>
#include <utility>

ImagePanel::ImagePanel(const QString &titulo, QWidget *parent)
    : QWidget(parent), imagemLabel(new QLabel(this)) {
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(18, 16, 18, 18);
    layout->setSpacing(12);

    auto *tituloLabel = new QLabel(titulo, this);
    tituloLabel->setObjectName("panelTitle");

    imagemLabel->setAlignment(Qt::AlignCenter);
    imagemLabel->setMinimumSize(320, 260);
    imagemLabel->setText("Nenhuma imagem carregada");
    imagemLabel->setObjectName("imageCanvas");

    layout->addWidget(tituloLabel);
    layout->addWidget(imagemLabel, 1);
    setObjectName("imagePanel");
}

void ImagePanel::setImage(const QImage &novaImagem) {
    imagem = novaImagem;
    atualizarImagem();
}

void ImagePanel::clearImage() {
    imagem = QImage();
    imagemLabel->setPixmap(QPixmap());
    imagemLabel->setText("Nenhuma imagem carregada");
}

void ImagePanel::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    atualizarImagem();
}

void ImagePanel::atualizarImagem() {
    if (imagem.isNull()) {
        return;
    }

    const QSize area = imagemLabel->size() - QSize(24, 24);
    imagemLabel->setText(QString());
    imagemLabel->setPixmap(QPixmap::fromImage(imagem).scaled(
        area, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    montarInterface();
    criarAtalhos();
    atualizarControles(false);
}

void MainWindow::montarInterface() {
    setWindowTitle("Laboratorio de Imagens | UFRGS");
    resize(1180, 720);
    setMinimumSize(900, 600);

    auto *central = new QWidget(this);
    auto *layoutPrincipal = new QVBoxLayout(central);
    layoutPrincipal->setContentsMargins(24, 22, 24, 20);
    layoutPrincipal->setSpacing(18);

    auto *cabecalho = new QHBoxLayout;
    auto *textosCabecalho = new QVBoxLayout;
    auto *titulo = new QLabel("Laboratorio de Imagens", central);
    titulo->setObjectName("mainTitle");
    auto *subtitulo = new QLabel(
        "Abra uma imagem JPEG e compare cada transformacao com o arquivo original.", central);
    subtitulo->setObjectName("subtitle");
    textosCabecalho->addWidget(titulo);
    textosCabecalho->addWidget(subtitulo);

    auto *abrirButton = new QPushButton("Abrir imagem", central);
    abrirButton->setObjectName("primaryButton");
    abrirButton->setToolTip("Abrir arquivo JPEG (Ctrl+O)");
    connect(abrirButton, &QPushButton::clicked, this, [this] { abrirImagem(); });

    salvarButton = new QPushButton("Salvar resultado", central);
    salvarButton->setToolTip("Salvar o resultado em JPEG (Ctrl+S)");
    connect(salvarButton, &QPushButton::clicked, this, [this] { salvarImagem(); });

    cabecalho->addLayout(textosCabecalho, 1);
    cabecalho->addWidget(abrirButton);
    cabecalho->addWidget(salvarButton);
    layoutPrincipal->addLayout(cabecalho);

    auto *barraFerramentas = new QFrame(central);
    barraFerramentas->setObjectName("toolbar");
    auto *layoutFerramentas = new QHBoxLayout(barraFerramentas);
    layoutFerramentas->setContentsMargins(14, 12, 14, 12);
    layoutFerramentas->setSpacing(10);

    auto *operacaoLabel = new QLabel("Operacao:", barraFerramentas);
    operacaoLabel->setObjectName("operationLabel");

    operacaoCombo = new QComboBox(barraFerramentas);
    operacaoCombo->setMinimumWidth(320);
    operacaoCombo->addItems({
        "Espelhar horizontal",
        "Espelhar vertical",
        "Converter para tons de cinza",
        "Quantizar tons",
        "Exibir histograma",
        "Ajustar brilho",
        "Ajustar contraste",
        "Aplicar negativo",
        "Reduzir imagem",
        "Girar 90 graus - horario",
        "Girar 90 graus - anti-horario",
        "Ampliar 2x",
        "Convolucao 3x3"
    });

    aplicarButton = new QPushButton("Aplicar", barraFerramentas);
    restaurarButton = new QPushButton("Restaurar original", barraFerramentas);

    connect(aplicarButton, &QPushButton::clicked, this,
            [this] { aplicarOperacaoSelecionada(); });
    connect(restaurarButton, &QPushButton::clicked, this,
            [this] { restaurarOriginal(); });

    layoutFerramentas->addWidget(operacaoLabel);
    layoutFerramentas->addWidget(operacaoCombo, 1);
    layoutFerramentas->addWidget(aplicarButton);
    layoutFerramentas->addWidget(restaurarButton);
    layoutPrincipal->addWidget(barraFerramentas);

    auto *imagensLayout = new QHBoxLayout;
    imagensLayout->setSpacing(18);
    painelOriginal = new ImagePanel("ORIGINAL", central);
    painelResultado = new ImagePanel("RESULTADO", central);
    imagensLayout->addWidget(painelOriginal, 1);
    imagensLayout->addWidget(painelResultado, 1);
    layoutPrincipal->addLayout(imagensLayout, 1);

    auto *rodape = new QHBoxLayout;
    nomeArquivoLabel = new QLabel("Abra uma imagem para comecar", central);
    detalhesLabel = new QLabel(central);
    nomeArquivoLabel->setObjectName("fileInfo");
    detalhesLabel->setObjectName("imageDetails");
    rodape->addWidget(nomeArquivoLabel, 1);
    rodape->addWidget(detalhesLabel);
    layoutPrincipal->addLayout(rodape);

    setCentralWidget(central);
    statusBar()->showMessage("Pronto");

    setStyleSheet(R"(
        QMainWindow, QWidget { background: #ffffff; color: #172033; font-family: "Inter", "Segoe UI", sans-serif; }
        QLabel#mainTitle { font-size: 27px; font-weight: 700; color: #10213b; }
        QLabel#subtitle { font-size: 13px; color: #667085; }
        QFrame#toolbar { background: #ffffff; border: 1px solid #dfe4ea; border-radius: 12px; }
        QWidget#imagePanel { background: #ffffff; border: 1px solid #dfe4ea; border-radius: 14px; }
        QLabel#panelTitle { border: none; color: #526079; font-size: 12px; font-weight: 700; letter-spacing: 1px; }
        QLabel#imageCanvas { background: #e9edf2; border: 1px dashed #c5ccd6; border-radius: 10px; color: #87909f; }
        QPushButton { background: #2f6fed; color: #ffffff; border: 1px solid #2f6fed; border-radius: 8px; padding: 10px 15px; font-weight: 600; }
        QPushButton:hover { background: #245fd3; border-color: #245fd3; color: #ffffff; }
        QPushButton:pressed { background: #194da8; border-color: #194da8; }
        QPushButton:disabled { background: #eef0f3; color: #9aa2af; border-color: #e0e3e7; }
        QPushButton#primaryButton { background: #2f6fed; color: white; border-color: #2f6fed; }
        QPushButton#primaryButton:hover { background: #245fd3; }
        QLabel#operationLabel { color: #344054; font-weight: 700; padding-right: 4px; }
        QComboBox { background: #ffffff; border: 1px solid #9aabd0; border-radius: 8px; padding: 9px 12px; color: #172033; }
        QComboBox:hover, QComboBox:focus { border: 1px solid #2f6fed; }
        QComboBox:disabled { background: #eef0f3; color: #9aa2af; border-color: #e0e3e7; }
        QLabel#fileInfo { color: #344054; font-weight: 600; }
        QLabel#imageDetails { color: #667085; }
        QStatusBar { background: #eef1f5; color: #667085; }
    )");
}

void MainWindow::aplicarOperacaoSelecionada() {
    if (dadosResultado.empty()) return;

    switch (operacaoCombo->currentIndex()) {
        case 0: espelharHorizontalmente(); break;
        case 1: espelharVerticalmente(); break;
        case 2: converterParaCinza(); break;
        case 3: quantizar(); break;
        case 4: calcularHistograma(); break;
        case 5: ajustarBrilho(); break;
        case 6: ajustarContraste(); break;
        case 7: aplicarNegativo(); break;
        case 8: reduzirImagem(); break;
        case 9: rotacionarHorario(); break;
        case 10: rotacionarAntiHorario(); break;
        case 11: ampliarImagem(); break;
        case 12: aplicarConvolucao(); break;
        default: break;
    }
}

void MainWindow::criarAtalhos() {
    auto adicionarAtalho = [this](const QKeySequence &tecla, auto acao) {
        auto *atalho = new QAction(this);
        atalho->setShortcut(tecla);
        addAction(atalho);
        connect(atalho, &QAction::triggered, this, acao);
    };

    adicionarAtalho(QKeySequence::Open, [this] { abrirImagem(); });
    adicionarAtalho(QKeySequence::Save, [this] { salvarImagem(); });
    adicionarAtalho(QKeySequence(Qt::Key_H), [this] { espelharHorizontalmente(); });
    adicionarAtalho(QKeySequence(Qt::Key_V), [this] { espelharVerticalmente(); });
    adicionarAtalho(QKeySequence(Qt::Key_G), [this] { converterParaCinza(); });
    adicionarAtalho(QKeySequence(Qt::Key_Q), [this] { quantizar(); });
    adicionarAtalho(QKeySequence(Qt::Key_R), [this] { restaurarOriginal(); });
}

void MainWindow::abrirImagem() {
    const QString pastaInicial = caminhoAtual.isEmpty()
        ? QDir::current().filePath("test_images")
        : QFileInfo(caminhoAtual).absolutePath();
    const QString caminho = QFileDialog::getOpenFileName(
        this, "Abrir imagem JPEG", pastaInicial, "Imagens JPEG (*.jpg *.jpeg);;Todos os arquivos (*)");
    if (caminho.isEmpty()) {
        return;
    }

    int canais = 0;
    unsigned char *carregada = stbi_load(caminho.toUtf8().constData(), &largura, &altura, &canais, 3);
    if (!carregada) {
        QMessageBox::critical(this, "Nao foi possivel abrir",
                              "O arquivo selecionado nao pode ser lido como uma imagem.");
        return;
    }

    const size_t tamanho = static_cast<size_t>(largura) * static_cast<size_t>(altura) * 3;
    dadosOriginais.assign(carregada, carregada + tamanho);
    stbi_image_free(carregada);
    dadosResultado = dadosOriginais;
    caminhoAtual = caminho;
    larguraOriginal = largura;
    alturaOriginal = altura;

    painelOriginal->setImage(criarQImage(dadosOriginais));
    painelResultado->setImage(criarQImage(dadosResultado));
    nomeArquivoLabel->setText(QFileInfo(caminho).fileName());
    detalhesLabel->setText(QString("%1 x %2 px  |  RGB").arg(largura).arg(altura));
    atualizarControles(true);
    statusBar()->showMessage("Imagem carregada com sucesso", 3500);
}

void MainWindow::salvarImagem() {
    if (dadosResultado.empty()) {
        return;
    }

    const QFileInfo origem(caminhoAtual);
    QString sugestao = origem.absolutePath() + "/" + origem.completeBaseName() + "_resultado.jpg";
    QString destino = QFileDialog::getSaveFileName(
        this, "Salvar resultado", sugestao, "Imagem JPEG (*.jpg *.jpeg)");
    if (destino.isEmpty()) {
        return;
    }
    if (!destino.endsWith(".jpg", Qt::CaseInsensitive) &&
        !destino.endsWith(".jpeg", Qt::CaseInsensitive)) {
        destino += ".jpg";
    }

    const int sucesso = stbi_write_jpg(
        destino.toUtf8().constData(), largura, altura, 3, dadosResultado.data(), 90);
    if (!sucesso) {
        QMessageBox::critical(this, "Erro ao salvar",
                              "Nao foi possivel gravar o arquivo JPEG escolhido.");
        return;
    }

    statusBar()->showMessage("Resultado salvo em " + QDir::toNativeSeparators(destino), 5000);
}

void MainWindow::espelharHorizontalmente() {
    if (dadosResultado.empty()) return;
    espelhar_img(dadosResultado.data(), largura, altura, 'h', 1);
    atualizarResultado("Espelhamento horizontal aplicado");
}

void MainWindow::espelharVerticalmente() {
    if (dadosResultado.empty()) return;
    espelhar_img(dadosResultado.data(), largura, altura, 'v', 1);
    atualizarResultado("Espelhamento vertical aplicado");
}

void MainWindow::converterParaCinza() {
    if (dadosResultado.empty()) return;
    conversao_cinza(dadosResultado.data(), largura, altura);
    atualizarResultado("Conversao para tons de cinza aplicada");
}

void MainWindow::quantizar() {
    if (dadosResultado.empty()) return;

    bool confirmado = false;
    const int numeroTons = QInputDialog::getInt(
        this,
        "Quantizacao de tons",
        "Numero maximo de tons:",
        8,
        1,
        256,
        1,
        &confirmado);
    if (!confirmado) {
        return;
    }

    quantizacao(dadosResultado.data(), largura, altura, numeroTons);
    atualizarResultado(QString("Quantizacao aplicada com ate %1 tons").arg(numeroTons));
}

void MainWindow::calcularHistograma() {
    if (dadosResultado.empty()) return;

    const std::vector<int> valores = hist(dadosResultado.data(), largura, altura);
    const QImage grafico = imagem_histograma(valores);
    atualizarResultado("Histograma calculado e exibido em uma nova janela");

    auto *janelaHistograma = new QDialog(this);
    janelaHistograma->setAttribute(Qt::WA_DeleteOnClose);
    janelaHistograma->setWindowTitle("Histograma - tons de cinza");
    auto *layout = new QVBoxLayout(janelaHistograma);
    auto *graficoLabel = new QLabel(janelaHistograma);
    graficoLabel->setPixmap(QPixmap::fromImage(grafico).scaled(
        640, 420, Qt::IgnoreAspectRatio, Qt::FastTransformation));
    layout->addWidget(graficoLabel);
    janelaHistograma->show();
}

void MainWindow::ajustarBrilho() {
    if (dadosResultado.empty()) return;

    bool confirmado = false;
    const int valor = QInputDialog::getInt(
        this, "Ajuste de brilho", "Valor a somar aos pixels (-255 a 255):",
        0, -255, 255, 1, &confirmado);
    if (!confirmado) return;

    brilho(dadosResultado.data(), largura, altura, valor);
    atualizarResultado(QString("Brilho ajustado em %1").arg(valor));
}

void MainWindow::ajustarContraste() {
    if (dadosResultado.empty()) return;

    bool confirmado = false;
    const int fator = QInputDialog::getInt(
        this, "Ajuste de contraste", "Fator multiplicador (1 a 255):",
        1, 1, 255, 1, &confirmado);
    if (!confirmado) return;

    contraste(dadosResultado.data(), largura, altura, fator);
    atualizarResultado(QString("Contraste multiplicado por %1").arg(fator));
}

void MainWindow::aplicarNegativo() {
    if (dadosResultado.empty()) return;

    negativo(dadosResultado.data(), largura, altura);
    atualizarResultado("Negativo aplicado");
}

void MainWindow::reduzirImagem() {
    if (dadosResultado.empty()) return;

    bool confirmouSx = false;
    const int sx = QInputDialog::getInt(
        this, "Reducao da imagem", "Fator horizontal sx:", 2, 1, 255, 1, &confirmouSx);
    if (!confirmouSx) return;

    bool confirmouSy = false;
    const int sy = QInputDialog::getInt(
        this, "Reducao da imagem", "Fator vertical sy:", 2, 1, 255, 1, &confirmouSy);
    if (!confirmouSy) return;

    const int novaLargura = (largura + sx - 1) / sx;
    const int novaAltura = (altura + sy - 1) / sy;
    zoomOut(dadosResultado.data(), largura, altura, sx, sy);
    dadosResultado.resize(static_cast<size_t>(novaLargura) * novaAltura * 3);
    largura = novaLargura;
    altura = novaAltura;
    atualizarResultado(QString("Imagem reduzida com sx=%1 e sy=%2").arg(sx).arg(sy));
}

void MainWindow::rotacionarHorario() {
    if (dadosResultado.empty()) return;

    rotacionar90(dadosResultado.data(), largura, altura, 1);
    std::swap(largura, altura);
    atualizarResultado("Rotacao de 90 graus no sentido horario aplicada");
}

void MainWindow::rotacionarAntiHorario() {
    if (dadosResultado.empty()) return;

    rotacionar90(dadosResultado.data(), largura, altura, -1);
    std::swap(largura, altura);
    atualizarResultado("Rotacao de 90 graus no sentido anti-horario aplicada");
}

void MainWindow::ampliarImagem() {
    if (dadosResultado.empty()) return;

    unsigned char *ampliada = ampliar(dadosResultado.data(), largura, altura);
    if (ampliada == nullptr) {
        QMessageBox::warning(this, "Erro na ampliacao",
                             "Nao foi possivel ampliar a imagem atual.");
        return;
    }

    int novaLargura = largura * 2 - 1;
    int novaAltura = altura * 2 - 1;
    size_t novoTamanho = static_cast<size_t>(novaLargura) * novaAltura * 3;
    dadosResultado.assign(ampliada, ampliada + novoTamanho);
    delete[] ampliada;

    largura = novaLargura;
    altura = novaAltura;
    atualizarResultado("Imagem ampliada por interpolacao linear 2x2");
}

void MainWindow::aplicarConvolucao() {
    if (dadosResultado.empty()) return;

    QDialog dialogo(this);
    dialogo.setWindowTitle("Convolucao com kernel 3x3");
    dialogo.resize(520, 430);

    auto *layoutPrincipal = new QVBoxLayout(&dialogo);
    auto *linhaPreset = new QHBoxLayout;
    auto *presetLabel = new QLabel("Filtro:", &dialogo);
    auto *presetCombo = new QComboBox(&dialogo);
    presetCombo->addItems({
        "Arbitrario",
        "Gaussiano - passa-baixas",
        "Laplaciano",
        "Passa-altas generico",
        "Prewitt Hx",
        "Prewitt Hy",
        "Sobel Hx",
        "Sobel Hy"
    });
    linhaPreset->addWidget(presetLabel);
    linhaPreset->addWidget(presetCombo, 1);
    layoutPrincipal->addLayout(linhaPreset);

    auto *instrucao = new QLabel(
        "Edite qualquer peso abaixo. O kernel sera rotacionado 180 graus "
        "durante a convolucao.", &dialogo);
    instrucao->setWordWrap(true);
    layoutPrincipal->addWidget(instrucao);

    auto *gradeKernel = new QGridLayout;
    QDoubleSpinBox *pesos[3][3];

    for (int linha = 0; linha < 3; linha++) {
        for (int coluna = 0; coluna < 3; coluna++) {
            pesos[linha][coluna] = new QDoubleSpinBox(&dialogo);
            pesos[linha][coluna]->setRange(-999.0, 999.0);
            pesos[linha][coluna]->setDecimals(4);
            pesos[linha][coluna]->setSingleStep(0.0625);
            pesos[linha][coluna]->setValue(
                linha == 1 && coluna == 1 ? 1.0 : 0.0);
            gradeKernel->addWidget(pesos[linha][coluna], linha, coluna);
        }
    }
    layoutPrincipal->addLayout(gradeKernel);

    auto *passaBaixas = new QCheckBox(
        "Aplicar separadamente aos canais RGB (filtro passa-baixas)", &dialogo);
    auto *adicionar127 = new QCheckBox(
        "Somar 127 antes do clampping (filtros de relevo)", &dialogo);
    layoutPrincipal->addWidget(passaBaixas);
    layoutPrincipal->addWidget(adicionar127);

    static const double kernels[7][9] = {
        {0.0625, 0.125, 0.0625, 0.125, 0.25, 0.125, 0.0625, 0.125, 0.0625},
        {0, -1, 0, -1, 4, -1, 0, -1, 0},
        {-1, -1, -1, -1, 8, -1, -1, -1, -1},
        {-1, 0, 1, -1, 0, 1, -1, 0, 1},
        {-1, -1, -1, 0, 0, 0, 1, 1, 1},
        {-1, 0, 1, -2, 0, 2, -1, 0, 1},
        {-1, -2, -1, 0, 0, 0, 1, 2, 1}
    };

    connect(presetCombo, &QComboBox::currentIndexChanged, &dialogo,
            [=](int indice) {
        if (indice == 0) return;

        const double *kernel = kernels[indice - 1];
        for (int linha = 0; linha < 3; linha++) {
            for (int coluna = 0; coluna < 3; coluna++) {
                pesos[linha][coluna]->setValue(kernel[linha * 3 + coluna]);
            }
        }

        passaBaixas->setChecked(indice == 1);
        adicionar127->setChecked(indice >= 4);
    });

    auto *botoes = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialogo);
    connect(botoes, &QDialogButtonBox::accepted, &dialogo, &QDialog::accept);
    connect(botoes, &QDialogButtonBox::rejected, &dialogo, &QDialog::reject);
    layoutPrincipal->addWidget(botoes);

    if (dialogo.exec() != QDialog::Accepted) {
        return;
    }

    double kernel[3][3];
    for (int linha = 0; linha < 3; linha++) {
        for (int coluna = 0; coluna < 3; coluna++) {
            kernel[linha][coluna] = pesos[linha][coluna]->value();
        }
    }

    convolucao(dadosResultado.data(), largura, altura, kernel,
               passaBaixas->isChecked(), adicionar127->isChecked());
    atualizarResultado("Convolucao aplicada: " + presetCombo->currentText());
}

void MainWindow::restaurarOriginal() {
    if (dadosOriginais.empty()) return;
    dadosResultado = dadosOriginais;
    largura = larguraOriginal;
    altura = alturaOriginal;
    atualizarResultado("Resultado restaurado para a imagem original");
}

void MainWindow::atualizarResultado(const QString &mensagem) {
    painelResultado->setImage(criarQImage(dadosResultado));
    detalhesLabel->setText(QString("%1 x %2 px  |  RGB").arg(largura).arg(altura));
    statusBar()->showMessage(mensagem, 3500);
}

void MainWindow::atualizarControles(bool habilitados) {
    salvarButton->setEnabled(habilitados);
    operacaoCombo->setEnabled(habilitados);
    aplicarButton->setEnabled(habilitados);
    restaurarButton->setEnabled(habilitados);
}

QImage MainWindow::criarQImage(const std::vector<unsigned char> &dados) const {
    if (dados.empty()) {
        return {};
    }
    return QImage(dados.data(), largura, altura, largura * 3, QImage::Format_RGB888).copy();
}
