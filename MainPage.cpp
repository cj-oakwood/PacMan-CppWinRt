#include "pch.h"
#include "MainPage.h"

using namespace winrt;
using namespace Windows::UI;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Shapes;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Controls;

namespace winrt::WinRtMaze::implementation
{
    MainPage::MainPage()
    {
        InitializeComponent();

        _creeper = Image();
        auto bi = Imaging::BitmapImage(Windows::Foundation::Uri(L"ms-appx://Assets/Images/creeper.png"));
        _creeper.Source(bi);
        _creeper.Height(100);
        _creeper.Visibility(Visibility::Collapsed);
        pacManGrid().Children().Append(_creeper);

        Reset();
    }

    int32_t MainPage::Dummy()
    {
        throw hresult_not_implemented();
    }

    void MainPage::Dummy(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }

    void MainPage::_startButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        m_sleepTimer = _slider().Value();

        _resetButton().IsEnabled(false);
        _startButton().IsEnabled(false);

        Grid().SetColumn(_ellipse, 0);
        Grid().SetRow(_ellipse, 0);

        auto t = Solve(m_gameArray2);
        t.then([this](bool /*r*/) {
            //UNREFERENCED_PARAMETER(r);
            _resetButton().IsEnabled(true);
            _startButton().IsEnabled(true);
        }, concurrency::cancellation_token::none(), concurrency::task_continuation_context::get_current_winrt_context());
    }

    void MainPage::_resetButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        pacManGrid().Children().Clear();

        Reset();
    }

    void MainPage::SetWalls(int seed, int maze[N][N])
    {
        srand(seed);
        auto length = N*2;
        for (auto i = 0; i < length; i++)
        {
            auto v = rand() % 7 + 1;
            auto h = rand() % 7;

            if ((v == 0 && h == 0) || (v == 7 && h == 7))
                continue;

            m_gameArray2[v][h] = 1;
        }

        UNREFERENCED_PARAMETER(maze);
    }

    concurrency::task<bool> MainPage::Solve(const std::array < std::array<int, N>, N> maze )
    {
        return concurrency::create_task([this, maze]
        {
            int sol[N][N]{};
            return SolveHelper(maze, 0, 0, sol);
        });
    }

    bool MainPage::SolveHelper(const std::array < std::array<int, N>, N> maze , const int x, const int y, int sol[N][N])
    {
        if (x == N - 1 && y == N - 1)
        {
            UpdateEllipseOnMap(x, y, (int)m_sleepTimer);
            sol[x][y] = 1;
            return true;
        }

        if (IsSafe(maze, x, y))
        {
            UpdateEllipseOnMap(x, y, (int)m_sleepTimer);

            if (SolveHelper(maze, x + 1, y, sol))
            {
                sol[x][y] = 1;
                return true;
            }
            else if (SolveHelper(maze, x, y + 1, sol))
            {
                sol[x][y] = 1;
                return true;
            }
            else
            {
                sol[x][y] = 0;
                return false;
            }
        }
        return false;
    }

    bool MainPage::IsSafe(const std::array < std::array<int, N>, N> maze , const int x, const int y)
    {
        if (x >= 0 && x < N && y >= 0 && y < N && maze[x][y] == 0)
            return true;

        return false;
    }

    void MainPage::UpdateEllipseOnMap(const int row, const int column, const int sleepTimer)
    {
        auto a = Windows::ApplicationModel::Core::CoreApplication::MainView();
        if (nullptr != a)
        {
            auto ignored = a.Dispatcher().RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal,
                Windows::UI::Core::DispatchedHandler([this, column, row]()
            {
                Grid().SetColumn(_ellipse, column);
                Grid().SetRow(_ellipse, row);

                _creeper.Visibility(Visibility::Visible);

                Grid().SetColumn(_creeper, 5);
                Grid().SetRow(_creeper, 5);
            }));


            Sleep((DWORD)sleepTimer);
        }
    }

    void MainPage::Reset()
    {
        time_t t(time(NULL));
        tm tm{};
        auto err = gmtime_s(&tm, &t);
        if (!err)
        {
            m_gameArray2 = std::array < std::array<int, N>, N> {0};

            auto seed = (tm.tm_hour+1) * (tm.tm_min+1) * (tm.tm_sec+1);
            _seed().Text(to_hstring(seed));

            SetWalls(seed, nullptr);

            for (auto i = 0; i < N; i++)
            {
                for (auto j = 0; j < N; j++)
                {
                    auto rectangle = Rectangle();
                    auto v = m_gameArray2[i][j];
                    if (v == 1)
                    {
                        auto brush = SolidColorBrush(Colors::Black());
                        rectangle.Fill(brush);
                    }

                    pacManGrid().Children().Append(rectangle);
                    Grid().SetColumn(rectangle, j);
                    Grid().SetRow(rectangle, i);
                }
            }

            _ellipse.Name(L"_ellipse");
            _ellipse.Fill(SolidColorBrush(Colors::Yellow()));
            pacManGrid().Children().Append(_ellipse);
            Grid().SetColumn(_ellipse, 0);
            Grid().SetRow(_ellipse, 0);

            auto end = TextBlock();
            end.Text(L"End");
            end.FontSize(32);
            end.Foreground(SolidColorBrush(Colors::Black()));

            pacManGrid().Children().Append(end);
            Grid().SetColumn(end, 7);
            Grid().SetRow(end, 7);
        }
    }
}
