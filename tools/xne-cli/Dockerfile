FROM mcr.microsoft.com/dotnet/runtime:7.0 AS base
WORKDIR /app

FROM mcr.microsoft.com/dotnet/sdk:7.0 AS build
WORKDIR /src
COPY ["xne-cli/xne-cli.csproj", "xne-cli/"]
RUN dotnet restore "xne-cli/xne-cli.csproj"
COPY . .
WORKDIR "/src/xne-cli"
RUN dotnet build "xne-cli.csproj" -c Release -o /app/build

FROM build AS publish
RUN dotnet publish "xne-cli.csproj" -c Release -o /app/publish /p:UseAppHost=false

FROM base AS final
WORKDIR /app
COPY --from=publish /app/publish .
ENTRYPOINT ["dotnet", "xne-cli.dll"]
