
// FilterDlg.cpp: файл реализации
//

#include "stdafx.h"
#include "Filter.h"
#include "FilterDlg.h"
#include "afxdialogex.h"

#include <ctime>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Диалоговое окно CFilterDlg



CFilterDlg::CFilterDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_FILTER_DIALOG, pParent)
	, percentNoise(10)
	, _A1(50)
	, _A2(150)
	, _A3(240)
	, _x1(10)
	, _y1(10)
	, _sx1(50)
	, _sy1(50)
	, _x2(150)
	, _y2(150)
	, _sx2(10)
	, _sy2(10)
	, _x3(70)
	, _y3(200)
	, _sx3(100)
	, _sy3(10)
	, _heightGauss(250)
	, _widthGauss(250)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFilterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PIC1, _picture1);
	DDX_Control(pDX, IDC_DPI, _dpiText);
	DDX_Control(pDX, IDC_DPI_NEW, _dpiTextNew);
	DDX_Text(pDX, IDC_EDIT1, percentNoise);
	DDX_Control(pDX, IDC_PIC2, _picture2);
	DDX_Text(pDX, IDC_A1, _A1);
	DDX_Text(pDX, IDC_A2, _A2);
	DDX_Text(pDX, IDC_A3, _A3);
	DDX_Text(pDX, IDC_X1, _x1);
	DDX_Text(pDX, IDC_Y1, _y1);
	DDX_Text(pDX, IDC_SIGMAX1, _sx1);
	DDX_Text(pDX, IDC_SIGMAY1, _sy1);
	DDX_Text(pDX, IDC_X2, _x2);
	DDX_Text(pDX, IDC_Y2, _y2);
	DDX_Text(pDX, IDC_SIGMAX2, _sx2);
	DDX_Text(pDX, IDC_SIGMAY2, _sy2);
	DDX_Text(pDX, IDC_X3, _x3);
	DDX_Text(pDX, IDC_Y3, _y3);
	DDX_Text(pDX, IDC_SIGMAX3, _sx3);
	DDX_Text(pDX, IDC_SIGMAY3, _sy3);
	DDX_Text(pDX, IDC_HEIGHTG, _heightGauss);
	DDX_Text(pDX, IDC_WIDTHG, _widthGauss);
}

BEGIN_MESSAGE_MAP(CFilterDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_LOAD, &CFilterDlg::OnBnClickedLoad)
	ON_BN_CLICKED(IDC_NOISE, &CFilterDlg::OnBnClickedNoise)
	ON_BN_CLICKED(IDC_SPECTRE, &CFilterDlg::OnBnClickedSpectre)
	ON_BN_CLICKED(IDC_GAUSS, &CFilterDlg::OnBnClickedGauss)
	ON_BN_CLICKED(IDC_ENERGY, &CFilterDlg::OnBnClickedEnergy)
	ON_BN_CLICKED(IDC_TOIMAGE, &CFilterDlg::OnBnClickedToimage)
END_MESSAGE_MAP()


// Обработчики сообщений CFilterDlg

BOOL CFilterDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Задает значок для этого диалогового окна.  Среда делает это автоматически,
	//  если главное окно приложения не является диалоговым
	SetIcon(m_hIcon, TRUE);			// Крупный значок
	SetIcon(m_hIcon, FALSE);		// Мелкий значок

	// TODO: добавьте дополнительную инициализацию

	srand(time(NULL));
	return TRUE;  // возврат значения TRUE, если фокус не передан элементу управления
}

// При добавлении кнопки свертывания в диалоговое окно нужно воспользоваться приведенным ниже кодом,
//  чтобы нарисовать значок.  Для приложений MFC, использующих модель документов или представлений,
//  это автоматически выполняется рабочей областью.

void CFilterDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // контекст устройства для рисования

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Выравнивание значка по центру клиентского прямоугольника
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Нарисуйте значок
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// Система вызывает эту функцию для получения отображения курсора при перемещении
//  свернутого окна.
HCURSOR CFilterDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CFilterDlg::OnBnClickedLoad()
{
	LoadPicture();
}

void CFilterDlg::LoadPicture()
{
	CFileDialog fd(true, NULL, NULL, OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY |
		OFN_LONGNAMES | OFN_PATHMUSTEXIST, _T("Image Files (*.bmp)|*.bmp|All Files (*.*)|*.*||"), NULL, 0, TRUE);

	if (fd.DoModal() != IDOK)
	{
		return;
	};

	CString pathBMP = fd.GetPathName();
	Bitmap bmp(pathBMP);
	int width = bmp.GetWidth();
	int height = bmp.GetHeight();

	int newwidth, newheight;
	CheckBin(width, newwidth);
	CheckBin(height, newheight);

	_image.clear();

	for (size_t i = 0; i < height; i++)
	{
		std::vector<double> bufPix;
		for (size_t j = 0; j < width; j++)
		{
			double value;
			Color color;
			bmp.GetPixel(j, height - i - 1, &color);
			value = 0.299*color.GetRed() + 0.587*color.GetGreen() + 0.114*color.GetBlue();
			bufPix.push_back(value);
		}
		_image.push_back(bufPix);
	}

	CheckSizeImage(_image, width, height, newwidth, newheight);
	_imageNoised = _image;
	_picture1.xmin = 0;
	_picture1.xmax = newwidth;
	_picture1.ymax = newheight;
	_picture1.ymin = 0;

	CString str, arg1, arg2;
	str = "Resolution: ";
	arg1.Format(_T("%d"), width);
	arg2.Format(_T("%d"), height);
	str += arg1 + "x" + arg2;
	_dpiText.SetWindowTextW(str);

	CString str2, arg3, arg4;
	str2 = "New resolution: ";
	arg3.Format(_T("%d"), newwidth);
	arg4.Format(_T("%d"), newheight);
	str2 += arg3 + "x" + arg4;
	_dpiTextNew.SetWindowTextW(str2);

	_picture1._image = &_image;
	_picture1.Update();
}

bool CFilterDlg::CheckBin(int value, int &newvalue)
{
	bool result;
	int tempVal = value;
	if (tempVal <= 0)
		return false;
	while ((tempVal % 2) == 0)
	{
		if ((tempVal /= 2) == 1)
		{
			newvalue = tempVal;
			return true;
		}
	}

	int initValue = 1;
	while (initValue <= value)
	{
		initValue *= 2;
	}
	newvalue = initValue;

	return false;
}

void CFilterDlg::CheckSizeImage(std::vector<std::vector<double>> &image,
	int oldWidth, int oldHeight, int width, int height)
{
	for (size_t i = 0; i < oldHeight; i++)
	{
		for (size_t j = oldWidth; j < width; j++)
		{
			image[i].push_back(0);
		}
	}

	for (size_t i = oldHeight; i < height; i++)
	{
		std::vector<double> bufVec;
		for (size_t j = 0; j < width; j++)
		{
			bufVec.push_back(0);
		}
		image.push_back(bufVec);
	}
}

void CFilterDlg::AddNoise()
{
	std::vector < std::vector < double >> noise;

	int width = _image[0].size();
	int height = _image.size();

	for (size_t i = 0; i < height; i++)
	{
		std::vector<double> buf;
		for (size_t j = 0; j < width; j++)
		{
			double M, ksi, noiseVal;
			M = rand() % 9 + 12;
			ksi = 0;
			for (int k = 1; k <= M; k++)
			{
				ksi += (double)((rand() % 21 - 10) / 10.);
			}
			noiseVal = ksi / M;
			buf.push_back(noiseVal);
		}
		noise.push_back(buf);
	}

	double EnergyImage = 0, EnergyNoise = 0;

#pragma omp parallel for
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			EnergyImage += _image[i][j] * _image[i][j];
			EnergyNoise += noise[i][j] * noise[i][j];
		}
	}

	double alfa = sqrt(EnergyImage / EnergyNoise * percentNoise / 100.f);
#pragma omp parallel for
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			_imageNoised[i][j] = _image[i][j] + alfa * noise[i][j];
		}
	}

	_picture1._image = &_imageNoised;

}

void CFilterDlg::OnBnClickedNoise()
{
	UpdateData(TRUE);
	AddNoise();
	_picture1.Update();
}

void CFilterDlg::Fourie1D(std::vector<cmplx> *data, int n, int is)
{
	int i, j, istep;
	int m, mmax;
	float r, r1, theta, w_r, w_i, temp_r, temp_i;
	float pi = 3.1415926f;

	r = pi * is;
	j = 0;
	for (i = 0; i < n; i++)
	{
		if (i < j)
		{
			temp_r = (*data)[j].real;
			temp_i = (*data)[j].image;
			(*data)[j].real = (*data)[i].real;
			(*data)[j].image = (*data)[i].image;
			(*data)[i].real = temp_r;
			(*data)[i].image = temp_i;
		}
		m = n >> 1;
		while (j >= m) { j -= m; m = (m + 1) / 2; }
		j += m;
	}
	mmax = 1;
	while (mmax < n)
	{
		istep = mmax << 1;
		r1 = r / (float)mmax;
		for (m = 0; m < mmax; m++)
		{
			theta = r1 * m;
			w_r = (float)cos((double)theta);
			w_i = (float)sin((double)theta);
			for (i = m; i < n; i += istep)
			{
				j = i + mmax;
				temp_r = w_r * (*data)[j].real - w_i * (*data)[j].image;
				temp_i = w_r * (*data)[j].image + w_i * (*data)[j].real;
				(*data)[j].real = (*data)[i].real - temp_r;
				(*data)[j].image = (*data)[i].image - temp_i;
				(*data)[i].real += temp_r;
				(*data)[i].image += temp_i;
			}
		}
		mmax = istep;
	}
	if (is > 0)
		for (i = 0; i < n; i++)
		{
			(*data)[i].real /= (float)n;
			(*data)[i].image /= (float)n;
		}


}

void CFilterDlg::Fourie2D(std::vector<std::vector<cmplx>> &data, int is)
{
	int width = data[0].size();
	int height = data.size();
#pragma omp parallel for
	for (int i = 0; i < height; i++)
	{
		Fourie1D(&data[i], width, is);
	}

	std::vector<std::vector<cmplx>> bufRes;
	for (int i = 0; i < width; i++)
	{
		std::vector<cmplx> buffer;
		for (int j = 0; j < height; j++)
		{
			buffer.push_back(data[j][i]);
		}
		bufRes.push_back(buffer);
	}

#pragma omp parallel for
	for (int i = 0; i < width; i++)
	{
		Fourie1D(&bufRes[i], height, is);
	}

	data.clear();
	data = bufRes;
}


void CFilterDlg::OnBnClickedSpectre()
{
	std::vector<std::vector<cmplx>> image;
	int width = _imageNoised[0].size();
	int height = _imageNoised.size();
	_imageSpectre.clear();

	for (int i = 0; i < height; i++)
	{
		std::vector<cmplx> buffer;
		for (int j = 0; j < width; j++)
		{
			cmplx value;
			value.image = 0;
			value.real = _imageNoised[i][j];
			buffer.push_back(value);
		}
		image.push_back(buffer);
	}
	Fourie2D(image, -1);
	_imageSpectreCMPLX = image;
	for (int i = 0; i < height; i++)
	{
		std::vector<double> buffer;
		for (int j = 0; j < width; j++)
		{
			buffer.push_back(image[j][i].image*image[j][i].image + image[j][i].real*image[j][i].real);
		}
		_imageSpectre.push_back(buffer);
	}

	_picture2.xmax = width;
	_picture2.xmin = 0;
	_picture2.ymax = height;
	_picture2.ymin = 0;

	_picture2._image = &_imageSpectre;
	_picture2.Update();

}

void CFilterDlg::GaussianDistribution()
{
	_image.clear();
	for (int i = 0; i < _heightGauss; i++)
	{
		std::vector<double> bufValues;
		for (int j = 0; j < _widthGauss; j++)
		{
			double value;
			value = (_A1 * exp(-((j - _x1)*(j - _x1) + (i - _y1)*(i - _y1)) / (_sx1*_sx1 + _sy1 * _sy1)) +
				_A2 * exp(-((j - _x2)*(j - _x2) + (i - _y2)*(i - _y2)) / (_sx2*_sx2 + _sy2 * _sy2)) +
				_A3 * exp(-((j - _x3)*(j - _x3) + (i - _y3)*(i - _y3)) / (_sx3*_sx3 + _sy3 * _sy3))) / (_A1 + _A2 + _A3)*255.f;
			bufValues.push_back(value);
		}
		_image.push_back(bufValues);
	}

	_picture1._image = &_image;
	_picture1.xmin = 0;
	_picture1.xmax = _widthGauss;
	_picture1.ymax = _heightGauss;
	_picture1.ymin = 0;

	_picture1.Update();

}

void CFilterDlg::OnBnClickedGauss()
{
	UpdateData(TRUE);
	GaussianDistribution();
	int newwidth, newheight, width = _image[0].size(), height = _image.size();
	CheckBin(_widthGauss, newwidth);
	CheckBin(_heightGauss, newheight);

	CheckSizeImage(_image, width, height, newwidth, newheight);
	_imageNoised = _image;
	_picture1.xmin = 0;
	_picture1.xmax = newwidth;
	_picture1.ymax = newheight;
	_picture1.ymin = 0;

	CString str, arg1, arg2;
	str = "Resolution: ";
	arg1.Format(_T("%d"), width);
	arg2.Format(_T("%d"), height);
	str += arg1 + "x" + arg2;
	_dpiText.SetWindowTextW(str);

	CString str2, arg3, arg4;
	str2 = "New resolution: ";
	arg3.Format(_T("%d"), newwidth);
	arg4.Format(_T("%d"), newheight);
	str2 += arg3 + "x" + arg4;
	_dpiTextNew.SetWindowTextW(str2);

	_picture1._image = &_image;
	_picture1.Update();
}

void CFilterDlg::EnergySelecter()
{
	const double limit = 0.99;

	double fullEnergy = 0;
	double height = _imageSpectreCMPLX.size(),
		width = _imageSpectreCMPLX[0].size();

	for (int i = 0; i < height / 2; i++)
	{
		for (int j = 0; j < width / 2; j++)
		{
			fullEnergy += _imageSpectreCMPLX[i][j].image*_imageSpectreCMPLX[i][j].image +
				_imageSpectreCMPLX[i][j].real*_imageSpectreCMPLX[i][j].real;
		}
	}

	int R = 0;
	for (int radius = 0; radius < width / 2; radius++)
	{
		double localEnergy = 0;
		for (int i = 0; i < height / 2; i++)
		{
			for (int j = 0; j < width / 2; j++)
			{
				if (sqrt(i*i + j * j) <= (double)radius)
				{
					localEnergy += _imageSpectreCMPLX[i][j].image*_imageSpectreCMPLX[i][j].image +
						_imageSpectreCMPLX[i][j].real*_imageSpectreCMPLX[i][j].real;
				}
			}
		}
		if (localEnergy >= fullEnergy * limit)
		{
			R = radius;
			break;
		}
	}

	for (int i = 0; i <= height / 2; i++)
	{
		for (int j = 0; j <= width / 2; j++)
		{
			if (sqrt(i*i + j * j) >= (double)R)
			{
				_imageSpectreCMPLX[i][j].image = 0;
				_imageSpectreCMPLX[i][j].real = 0;
			}
		}
	}

	for (int i = 0; i <= height / 2; i++)
	{
		for (int j = width - 1; j >= width / 2; j--)
		{
			if (sqrt(i*i + (width - 1 - j) *(width -1  - j)) >= (double)R)
			{
				_imageSpectreCMPLX[i][j].image = 0;
				_imageSpectreCMPLX[i][j].real = 0;
			}
		}
	}

	for (int i = height -1; i >= height / 2; i--)
	{
		for (int j = 0; j <= width / 2; j++)
		{
			if (sqrt((height -1-i)*(height - 1 - i) + j * j) >= (double)R)
			{
				_imageSpectreCMPLX[i][j].image = 0;
				_imageSpectreCMPLX[i][j].real = 0;
			}
		}
	}

	for (int i = height - 1; i >= height / 2; i--)
	{
		for (int j = width - 1; j >= width / 2; j--)
		{
			if (sqrt((height - 1 - i)*(height - 1 - i) + (width - 1- j) * (width - 1 - j)) >= (double)R)
			{
				_imageSpectreCMPLX[i][j].image = 0;
				_imageSpectreCMPLX[i][j].real = 0;
			}
		}
	}

	_imageSpectre.clear();
	for (int i = 0; i < height; i++)
	{
		std::vector<double> buffer;
		for (int j = 0; j < width; j++)
		{
			buffer.push_back(_imageSpectreCMPLX[j][i].image*_imageSpectreCMPLX[j][i].image 
				+ _imageSpectreCMPLX[j][i].real*_imageSpectreCMPLX[j][i].real);
		}
		_imageSpectre.push_back(buffer);
	}


}

void CFilterDlg::OnBnClickedEnergy()
{
	EnergySelecter();
	_picture2.Update();
}


void CFilterDlg::OnBnClickedToimage()
{
	Fourie2D(_imageSpectreCMPLX, 1);
	double height = _imageSpectreCMPLX.size(),
		width = _imageSpectreCMPLX[0].size();
	_imageSpectre.clear();
	for (int i = 0; i < height; i++)
	{
		std::vector<double> buffer;
		for (int j = 0; j < width; j++)
		{
			buffer.push_back(_imageSpectreCMPLX[i][j].real);
		}
		_imageSpectre.push_back(buffer);
	}
	_picture2.Update();
}
